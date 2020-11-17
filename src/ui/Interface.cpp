#include <algorithm>
#include <iomanip>
#include <set>

#include "haunted/core/Defs.h"
#include "haunted/core/Key.h"
#include "pingpong/core/Defs.h"
#include "pingpong/core/Channel.h"
#include "pingpong/core/User.h"
#include "pingpong/events/Event.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Options.h"
#include "spjalla/core/Util.h"

#include "spjalla/config/Config.h"

#include "spjalla/events/WindowChanged.h"
#include "spjalla/events/WindowClosed.h"

#include "spjalla/lines/Basic.h"
#include "spjalla/lines/Chanlist.h"
#include "spjalla/lines/Overlay.h"
#include "spjalla/lines/Userlist.h"

namespace Spjalla::UI {
	Interface::Interface(Haunted::Terminal &term_, Client &parent_):
               terminal(&term_), parent(&parent_), renderer(parent_.cache) {
		initBasic();
		initSwappo();
		initExpando();

		input->focus();
		updateStatusbar();
		updateOverlay();
		setInterrupt();
	}


// Private instance methods


	void Interface::initBasic() {
		input = new Haunted::UI::TextInput(terminal);
		input->setName("input");
		input->setPrefix(ansi::dim(">> "));
	
		titlebar = new Haunted::UI::Label(terminal);
		titlebar->setName("titlebar");

		statusbar = new Haunted::UI::Label(terminal);
		statusbar->setName("statusbar");
	}

	void Interface::initSwappo() {
		overlay = new Window("overlay");
		overlay->ignoreIndex = true;
		overlay->type = WindowType::Overlay;
		overlay->setTerminal(terminal);
		overlay->setName("overlay_window");
		windows.push_front(overlay);
		overlay->keyFunction = [&](const Haunted::Key &k) {
			if (k == Haunted::KeyType::Mouse)
				return true;
			toggleOverlay();
			return k == Config::Keys::toggleOverlay || input->onKey(k);
		};

		statusWindow = new Window("status");
		statusWindow->type = WindowType::Status;
		statusWindow->setTerminal(terminal);
		statusWindow->setName("statusWindow");
		statusWindow->setAutoscroll(true);
		activeWindow = statusWindow;
		windows.push_front(statusWindow);

		swappo = new Haunted::UI::Boxes::SwapBox(terminal, {}, {activeWindow, overlay});
		swappo->setName("swappo");
	}

	void Interface::initExpando() {
		expando = new Haunted::UI::Boxes::ExpandoBox(terminal, terminal->getPosition(),
			Haunted::UI::Boxes::BoxOrientation::Vertical, {{titlebar, 1}, {swappo, -1}, {statusbar, 1}, {input, 1}});
		expando->setName("expando");
		terminal->setRoot(expando);
		expando->keyFunction = [&](const Haunted::Key &k) { return onKey(k); };
	}

	void Interface::removeWindow(Window *win) {
		if (win->getParent() != swappo)
			throw std::invalid_argument("Can't remove window: not a child of swappo.");

		// Remove the window from the windows list.
		auto iter = std::find(windows.begin(), windows.end(), win);
		if (iter != windows.end())
			windows.erase(iter);

		if (activeWindow == win)
			focusWindow(windows.front());

		win->unnotify();
		PingPong::Events::dispatch<Events::WindowClosedEvent>(win);
		swappo->removeChild(win);

		delete win;
	}

	void Interface::updateOverlay(const std::shared_ptr<PingPong::Channel> &chan) {
		overlay->clearLines();
		*overlay += Haunted::UI::SimpleLine(ansi::bold(chan->name) + " [" + chan->modeString() + "]");
		chan->users.sort([&](std::shared_ptr<PingPong::User> left, std::shared_ptr<PingPong::User> right) -> bool {
			const PingPong::HatSet &left_hats = chan->getHats(left), &right_hats = chan->getHats(right);
			if (left_hats < right_hats)
				return false;

			if (left_hats > right_hats)
				return true;

			return formicine::util::lower(left->name) < formicine::util::lower(right->name);
		});

		size_t longest_hats = 0;
		for (std::shared_ptr<PingPong::User> user: chan->users)
			longest_hats = std::max(longest_hats, chan->getHats(user).size());

		for (std::shared_ptr<PingPong::User> user: chan->users)
			*overlay += Lines::UserlistLine(parent, chan, user, longest_hats);
	}

	void Interface::updateOverlay(const std::shared_ptr<PingPong::User> &user) {
		overlay->clearLines();
		*overlay += Haunted::UI::SimpleLine(ansi::bold(user->name));
		for (std::weak_ptr<PingPong::Channel> chan: user->channels)
			*overlay += Lines::ChanlistLine(parent, user, chan.lock());
	}

	Haunted::UI::Container::Type::iterator Interface::windowIterator() const {
		auto iter = std::find(swappo->begin(), swappo->end(), activeWindow);
		if (iter == swappo->end())
			throw std::runtime_error("The active window isn't a child of swappo");
		return iter;
	}

	void Interface::updateTitlebar(const std::shared_ptr<PingPong::Channel> &chan) {
		if (!chan) {
			DBG("chan is null in updateTitlebar");
			return;
		}

		titlebar->setText(" " + std::string(chan->topic));
	}

	bool Interface::canRemove(Window *win) const {
		if (win == nullptr)
			win = activeWindow;

		return !(win->isStatus() || win->isOverlay() || (win->isChannel() && !win->isDead()));
	}

	void Interface::setInterrupt() {
		terminal->onInterrupt = [this]() {
			for (auto &pair: parent->getIRC().servers)
				pair.second->kill();

			return true;
		};
	}


// Public instance methods


	void Interface::draw() {
		terminal->draw();
	}

	void Interface::init() {
		terminal->watchSize();
		renderer.initStrNodes();
	}

	void Interface::postinit() {
		renderer.moreStrNodes();
	}

	void Interface::log(const std::string &line, Window *win) {
		log(Lines::BasicLine(parent, line, 0), win);
	}

	void Interface::log(const std::string &line, const std::string &window_name) {
		log(Lines::BasicLine(parent, line, 0), window_name);
	}

	void Interface::log(const std::exception &err) {
		log(ansi::red(Haunted::Util::demangleObject(err)) + ": "_d + ansi::bold(err.what()));
	}

	void Interface::focusWindow(Window *win) {
		if (activeWindow == overlay) {
			beforeOverlay = nullptr;
			input->focus();
		}

		if (win == nullptr)
			win = statusWindow;

		if (win == activeWindow)
			return;

		UI::Window *old_active = activeWindow;
		swappo->setActive(activeWindow = win);

		PingPong::Events::dispatch<Events::WindowChangedEvent>(old_active, win);
		win->unnotify();

		if (win == overlay) {
			updateOverlay();
			overlay->focus();
		} else if (win != windows.front()) {
			// Move the window to the front of the windows list.
			auto iter = std::find(windows.begin(), windows.end(), win);
			if (iter == windows.end())
				throw std::runtime_error("Window is inexplicably not in the windows list");
			windows.erase(iter);
			windows.push_front(win);
		}

		updateStatusbar();
		updateTitlebar();
	}

	void Interface::focusWindow(const std::string &window_name) {
		focusWindow(getWindow(window_name));
	}

	bool Interface::focusWindow(size_t index) {
		for (Haunted::UI::Control *ctrl: swappo->getChildren()) {
			ssize_t control_index = ctrl->getIndex();
			if (0 <= control_index && static_cast<size_t>(control_index) == index) {
				focusWindow(dynamic_cast<Window *>(ctrl));
				return true;
			}
		}

		return false;
	}

	ssize_t Interface::moveWindow(Window *window, size_t newpos) {
		if (!window)
			return -1;

		std::deque<Haunted::UI::Control *> &controls = swappo->getChildren();

		if (std::find(controls.begin(), controls.end(), window) == controls.end())
			return -1;

		auto overlay_iter = std::find(controls.begin(), controls.end(), overlay);
		bool move_overlay = overlay_iter + 1 != controls.end();
		if (move_overlay)
			controls.erase(overlay_iter);

		controls.erase(std::find(controls.begin(), controls.end(), window));
		newpos = std::min(newpos, controls.size());
		controls.insert(std::next(controls.begin(), newpos), window);

		if (move_overlay)
			controls.push_back(overlay);

		updateStatusbar();
		return newpos;
	}

	Window * Interface::getWindow(const std::string &window_name, bool create, WindowType type) {
		if (window_name == "status") {
			if (statusWindow == nullptr && create)
				statusWindow = newWindow("status", WindowType::Status);
			return statusWindow;
		}

		if (swappo->empty())
			return nullptr;

		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (window && window->windowName == window_name)
				return window;
		}

		return create? newWindow(window_name, type) : nullptr;
	}

	Window * Interface::getWindow(const std::shared_ptr<PingPong::Channel> &channel, bool create) {
		if (!channel)
			return nullptr;

		// First, search for a window that has a matching channel pointer.
		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (window && window->server == channel->server && window->channel == channel)
				return window;
		}

		// If there's no window with a matching pointer, check the name case-insensitively.
		const std::string lower = channel->server->id + "/" + formicine::util::lower(channel->name);
		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (formicine::util::lower(window->windowName) == lower)
				return window;
		}

		// If we couldn't find anything, optionally create a window with the correct characteristics.
		if (create) {
			Window *window = newWindow(channel->server->id + "/" + channel->name, WindowType::Channel);
			window->channel = channel;
			window->server = channel->server;
			return window;
		}

		return nullptr;
	}

	Window * Interface::getWindow(const std::shared_ptr<PingPong::User> &user, bool create) {
		if (!user)
			return nullptr;

		// First, search for a window that has a matching channel pointer.
		for (Haunted::UI::Control *ctrl: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(ctrl);
			if (window && window->server == user->server && window->user == user)
				return window;
		}

		// If there's no window with a matching pointer, check the name case-insensitively.
		const std::string lower = user->server->id + "/" + formicine::util::lower(user->name);
		for (Haunted::UI::Control *ctrl: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(ctrl);
			if (formicine::util::lower(window->windowName) == lower)
				return window;
		}

		// If we couldn't find anything, optionally create a window with the correct characteristics.
		if (create) {
			Window *window = newWindow(user->server->id + "/" + user->name, WindowType::User);
			window->user = user;
			window->server = user->server;
			return window;
		}

		return nullptr;
	}

	Window * Interface::newWindow(const std::string &name, WindowType type) {
		Window *window = newWindow<Window>(name);
		window->type = type;
		return window;
	}

	void Interface::nextServer() {
		if (activeWindow != statusWindow)
			return;

		if (!parent->activeServer() && !parent->irc.servers.empty()) {
			parent->irc.activeServer = parent->irc.servers.begin()->second;
			return;
		}

		if (parent->irc.servers.size() < 2)
			return;

		const auto &servers = parent->irc.serverOrder;
		auto iter = std::find(servers.begin(), servers.end(), parent->activeServer());
		if (iter == servers.end() || ++iter == servers.end())
			iter = servers.begin();
		parent->irc.activeServer = *iter;
		log(Lines::notice + "Switched to " + ansi::bold((*iter)->id) + ".");
		updateStatusbar();
		updateTitlebar();
	}

	void Interface::nextWindow() {
#ifdef OVERLAY_PREVENTS_SWAPPING
		if (activeWindow == overlay)
			return;
#endif
		if (swappo->empty()) {
			activeWindow = nullptr;
		} else if (!activeWindow) {
			focusWindow(dynamic_cast<Window *>(swappo->getChildren().at(0)));
		} else {
			auto iter = windowIterator();
			if (swappo->size() > 1 || *iter != overlay) {
				// If the only window is the overlay, then this loop would get stuck. This shouldn't be possible
				// (the status window should always be a child of swappo), but it can't hurt to prevent it anyway.
				do {
					if (++iter == swappo->end())
						iter = swappo->begin();
				} while (*iter == overlay); // Skip the overlay.
				focusWindow(dynamic_cast<Window *>(*iter));
			}
		}
	}

	void Interface::previousWindow() {
#ifdef OVERLAY_PREVENTS_SWAPPING
		if (activeWindow == overlay)
			return;
#endif
		if (swappo->empty()) {
			activeWindow = nullptr;
		} else if (!activeWindow) {
			focusWindow(dynamic_cast<Window *>(swappo->getChildren().at(0)));
		} else {
			auto iter = windowIterator();
			if (swappo->size() > 1 || *iter != overlay) {
				do {
					if (iter == swappo->begin())
						iter = swappo->end();
					--iter;
				} while (*iter == overlay);
				focusWindow(dynamic_cast<Window *>(*iter));
			}
		}
	}

	void Interface::toggleMouse() {
		DBG("Toggling mouse.");
		if (terminal->mouse() == Haunted::MouseMode::None) {
			terminal->mouse(Haunted::MouseMode::Motion);
		} else {
			terminal->mouse(Haunted::MouseMode::None);
		}
	}

	void Interface::updateStatusbar() {
		Window *window = activeWindow == overlay? beforeOverlay : activeWindow;

		if (updateStatusbarFunction) {
			updateStatusbarFunction(window);
		} else if (!window) {
			statusbar->setText("[?]");
		} else if (window->isStatus()) {
			statusbar->setText("["_d + window->windowName + "] ["_d + parent->activeServerID() + "]"_d);
		} else if (window->isChannel()) {
			statusbar->setText("["_d + parent->activeServerID() + "] ["_d
			                    + Util::colorizeIfDead(window->channel->name, window) + "]"_d);
		} else if (window->isUser()) {
			statusbar->setText("["_d + parent->activeServerID() + "] ["_d
			                    + Util::colorizeIfDead(window->user->name, window) + "]"_d);
		} else {
			statusbar->setText("[" + ansi::bold(window->windowName) + "]");
		}
	}

	void Interface::updateOverlay() {
		overlay->clearLines();

		if (beforeOverlay == nullptr)
			return;

		if (beforeOverlay == statusWindow) {
			*overlay += Haunted::UI::SimpleLine(ansi::bold("Servers"));
			for (PingPong::Server *server: parent->irc.serverOrder) {
				server->sortChannels();
				if (server->isActive()) {
					*overlay += Lines::StatusServerLine(parent, server);
					for (std::shared_ptr<PingPong::Channel> channel: server->channels)
						*overlay += Lines::StatusChannelLine(parent, channel);
				}
			}

			return;
		}

		WindowType type = beforeOverlay->type;
		if (type == WindowType::Channel)
			updateOverlay(beforeOverlay->channel);
		else if (type == WindowType::User)
			updateOverlay(beforeOverlay->user);

		overlay->draw();
	}

	void Interface::updateTitlebar() {
		WindowType type = activeWindow->type;
		if (type == WindowType::Channel) {
			updateTitlebar(activeWindow->channel);
		} else {
			titlebar->clear();
		}
	}

	Window * Interface::toggleOverlay() {
		if (activeWindow == overlay) {
			Window *old_active = activeWindow;
			focusWindow(beforeOverlay);
			return old_active;
		}

		beforeOverlay = activeWindow;
		focusWindow(overlay);
		return beforeOverlay;
	}

	std::deque<Window *> Interface::windowsForUser(std::shared_ptr<PingPong::User> user) const {
		if (swappo->empty())
			return {};

		std::deque<Window *> found {};

		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (!window)
				continue;

			if ((window->isUser() && *user == *window->user) || (window->isChannel() && window->channel->hasUser(user)))
				found.push_back(window);
		}

		return found;
	}

	Window * Interface::windowForChannel(std::shared_ptr<PingPong::Channel> channel) const {
		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (window && window->channel == channel)
				return window;
		}

		return nullptr;
	}

	std::shared_ptr<PingPong::Channel> Interface::getActiveChannel() const {
		Window *window = beforeOverlay? beforeOverlay : activeWindow;
		return window->isChannel()? window->channel : nullptr;
	}

	std::shared_ptr<PingPong::User> Interface::getActiveUser() const {
		return activeWindow->isUser()? activeWindow->user : nullptr;
	}

	bool Interface::isActive(const Window *window) const {
		if (!window)
			return false;

		return activeWindow == window || beforeOverlay == window;
	}

	bool Interface::overlayVisible() const {
		return activeWindow == overlay;
	}

	void Interface::scrollPage(bool up) {
		if (activeWindow == overlay)
			return;

		activeWindow->vscroll(std::max(1, activeWindow->getPosition().height / 2) * (up? -1 : 1));
	}

	bool Interface::onKey(const Haunted::Key &key) {
		Haunted::Key copy = key;
		if (!parent->beforeKey(copy))
			return false;

		if (copy == Config::Keys::toggleOverlay) {
			toggleOverlay();
		} else if (copy == Config::Keys::switchServer) {
			nextServer();
		} else if (copy == Config::Keys::nextWindow) {
			nextWindow();
		} else if (copy == Config::Keys::previousWindow) {
			previousWindow();
		} else if (copy == Config::Keys::toggleMouse) {
			toggleMouse();
		} else if (copy == Haunted::KeyMod::Ctrl) {
			switch (copy.type) {
				case Haunted::KeyType::g: log("Active server: " + parent->activeServerID()); break;
				case Haunted::KeyType::r:
					if (activeWindow)
						DBG("v == " << activeWindow->getVoffset() << ", autoscroll == "
							<< (activeWindow->getAutoscroll()? "true" : "false"));
					break;
				case Haunted::KeyType::m: {
					if (activeWindow->getAutoscroll())
						DBG("autoscroll: true -> false");
					else
						DBG("autoscroll: false -> true");
					activeWindow->setAutoscroll(!activeWindow->getAutoscroll());
					break;
				}
				default: return false;
			}

			return true;
		} else if (copy == Haunted::KeyMod::Shift) {
			if (!activeWindow)
				return false;
			
			switch (copy.type) {
				case Haunted::KeyType::UpArrow:
					activeWindow->vscroll(-1);
					break;
				case Haunted::KeyType::DownArrow: {
					activeWindow->vscroll(1); 
					const int total = activeWindow->totalRows(), height = activeWindow->getPosition().height,
					          voffset = activeWindow->getVoffset();
					if (total == height + voffset)
						activeWindow->setAutoscroll(true);
					break;
				}
				case Haunted::KeyType::LeftArrow:
					activeWindow->setAutoscroll(true);
					break;
				default: return false;
			}

			return true;
		} else if (copy == Haunted::KeyMod::Alt) {
			switch (copy.type) {
				case Haunted::KeyType::_1: focusWindow(static_cast<size_t>(0)); return true;
				case Haunted::KeyType::_2: focusWindow(1);  return true;
				case Haunted::KeyType::_3: focusWindow(2);  return true;
				case Haunted::KeyType::_4: focusWindow(3);  return true;
				case Haunted::KeyType::_5: focusWindow(4);  return true;
				case Haunted::KeyType::_6: focusWindow(5);  return true;
				case Haunted::KeyType::_7: focusWindow(6);  return true;
				case Haunted::KeyType::_8: focusWindow(7);  return true;
				case Haunted::KeyType::_9: focusWindow(8);  return true;
				case Haunted::KeyType::_0: focusWindow(9);  return true;
				case Haunted::KeyType::q:  focusWindow(10); return true;
				case Haunted::KeyType::w:  focusWindow(11); return true;
				case Haunted::KeyType::e:  focusWindow(12); return true;
				case Haunted::KeyType::r:  focusWindow(13); return true;
				case Haunted::KeyType::t:  focusWindow(14); return true;
				case Haunted::KeyType::y:  focusWindow(15); return true;
				case Haunted::KeyType::u:  focusWindow(16); return true;
				case Haunted::KeyType::i:  focusWindow(17); return true;
				case Haunted::KeyType::o:  focusWindow(18); return true;
				case Haunted::KeyType::p:  focusWindow(19); return true;
				default: return false;
			}
		} else if (copy.mods.none()) {
			switch (copy.type) {
				case Haunted::KeyType::PageDown:
				case Haunted::KeyType::PageUp:
					scrollPage(copy == Haunted::KeyType::PageUp);
					return true;
				case Haunted::KeyType::Tab:
					parent->tabComplete();
					return true;
				default:;
			}
		}

		return false;
	}

	void Interface::setInput(const std::string &str) {
		input->setText(str);
	}

	std::string Interface::getInput() const {
		return input->getText();
	}

	void Interface::setScrollBuffer(unsigned int new_scroll_buffer) {
		scrollBuffer = new_scroll_buffer;
		for (Haunted::UI::Control *control: swappo->getChildren()) {
			Window *window = dynamic_cast<Window *>(control);
			if (window)
				window->scrollBuffer = scrollBuffer;
		}
	}

	void Interface::setBarForeground(ansi::color fg) {
		statusbar->setForeground(fg);
		titlebar->setForeground(fg);
		statusbar->draw();
		titlebar->draw();
	}

	void Interface::setBarBackground(ansi::color bg) {
		statusbar->setBackground(bg);
		titlebar->setBackground(bg);
		statusbar->draw();
		titlebar->draw();
	}

	void Interface::setOverlayForeground(ansi::color fg) {
		overlay->setForeground(fg);
		overlay->draw();
	}

	void Interface::setOverlayBackground(ansi::color bg) {
		overlay->setBackground(bg);
		overlay->draw();
	}

	void Interface::setInputForeground(ansi::color fg) {
		input->setForeground(fg);
		input->draw();
	}

	void Interface::setInputBackground(ansi::color bg) {
		input->setBackground(bg);
		input->draw();
	}
}
