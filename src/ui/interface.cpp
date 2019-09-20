#include <iomanip>
#include <memory>
#include <set>
#include <thread>

#include <csignal>

#include "ui/interface.h"
#include "lib/haunted/core/hdefs.h"
#include "lib/haunted/core/key.h"
#include "lib/pingpong/core/ppdefs.h"
#include "lib/pingpong/core/channel.h"
#include "lib/pingpong/core/user.h"
#include "lines/chanlist.h"
#include "lines/overlay.h"
#include "lines/userlist.h"

#include "core/client.h"
#include "core/config.h"
#include "core/spopt.h"

namespace spjalla::ui {
	interface::interface(haunted::terminal *term_, client *parent_): term(term_), parent(parent_) {
		init_basic();
		init_swappo();
		init_expando();
		init_colors();

		input->focus();
		update_statusbar();
		update_overlay();
	}


// Private instance methods


	void interface::init_basic() {
		input = new haunted::ui::textinput(term);
		input->set_name("input");
	
		titlebar = new haunted::ui::label(term);
		titlebar->set_name("titlebar");

		statusbar = new haunted::ui::label(term);
		statusbar->set_name("statusbar");
	}

	void interface::init_swappo() {
		overlay = new window("overlay");
		overlay->data = {window_type::overlay};
		overlay->set_terminal(term);
		overlay->set_name("overlay_window");
		windows.push_front(overlay);
		overlay->key_fn = [&](const haunted::key &k) {
			toggle_overlay();
			return k == config::keys::toggle_overlay || input->on_key(k);
		};

		status_window = new window("status");
		status_window->data = {window_type::status};
		status_window->set_terminal(term);
		status_window->set_name("status_window");
		status_window->set_voffset(-1);
		active_window = status_window;
		windows.push_front(status_window);

		swappo = new haunted::ui::boxes::swapbox(term, {}, {active_window, overlay});
		swappo->set_name("swappo");
	}

	void interface::init_expando() {
		expando = new haunted::ui::boxes::expandobox(term, term->get_position(),
			haunted::ui::boxes::box_orientation::vertical, {{titlebar, 1}, {swappo, -1}, {statusbar, 1}, {input, 1}});
		expando->set_name("expando");
		term->set_root(expando);
		expando->key_fn = [&](const haunted::key &k) { return on_key(k); };
	}

	void interface::init_colors() {
		overlay->set_colors(ansi::color::white, ansi::color::verydark);
		titlebar->set_colors(ansi::color::white, ansi::color::blue);
		statusbar->set_colors(ansi::color::white, ansi::color::blue);
		// input->set_colors(ansi::color::normal, ansi::color::red);
		// active_window->set_colors(ansi::color::normal, ansi::color::magenta);
	}

	window * interface::get_window(const std::string &window_name, bool create, window_type type) {
		if (window_name == "status") {
			if (status_window == nullptr && create)
				status_window = new_window("status", window_type::status);
			return status_window;
		}

		if (swappo->empty())
			return nullptr;

		for (haunted::ui::control *ctrl: swappo->get_children()) {
			window *win = dynamic_cast<window *>(ctrl);
			if (win && win->window_name == window_name)
				return win;
		}

		return create? new_window(window_name, type) : nullptr;
	}

	window * interface::get_window(const std::shared_ptr<pingpong::channel> &chan, bool create) {
		if (!chan)
			return nullptr;

		const std::string name = chan->serv->hostname + "/" + chan->name;
		window *win = get_window(name, false);

		if (create && !win) {
			win = new_window(name, window_type::channel);
			win->data.chan = chan;
			win->data.serv = chan->serv;
		}

		return win;
	}

	window * interface::get_window(const std::shared_ptr<pingpong::user> &user, bool create) {
		if (!user)
			return nullptr;

		const std::string name = user->serv->hostname + "/" + user->name;
		window *win = get_window(name, false);

		if (create && !win) {
			win = new_window(name, window_type::user);
			win->data.user = user;
			win->data.serv = user->serv;
		}

		return win;
	}

	window * interface::new_window(const std::string &name, window_type type) {
		static size_t win_count = 0;
		window *win = new window(swappo, swappo->get_position(), name);
		win->data.type = type;
		win->set_name("window" + std::to_string(++win_count));
		win->set_terminal(nullptr); // inactive windows are marked by their null terminals
		win->set_voffset(-1);
		return win;
	}

	void interface::remove_window(window *win) {
		if (win->get_parent() != swappo)
			throw std::invalid_argument("Can't remove window: not a child of swappo.");

		if (active_window == win)
			focus_window(status_window);

		swappo->remove_child(win);
		delete win;
	}

	void interface::update_overlay(const std::shared_ptr<pingpong::channel> &chan) {
		overlay->clear_lines();
		*overlay += haunted::ui::simpleline(ansi::bold(chan->name) + " [" + chan->mode_str() + "]");
		chan->users.sort([&](std::shared_ptr<pingpong::user> left, std::shared_ptr<pingpong::user> right) -> bool {
			return left->name < right->name;
		});

		for (std::shared_ptr<pingpong::user> user: chan->users)
			*overlay += spjalla::lines::userlist_line(chan, user);
	}

	void interface::update_overlay(const std::shared_ptr<pingpong::user> &user) {
		overlay->clear_lines();
		*overlay += haunted::ui::simpleline(ansi::bold(user->name));
		for (std::weak_ptr<pingpong::channel> chan: user->channels)
			*overlay += spjalla::lines::chanlist_line(user, chan.lock());
	}

	std::vector<haunted::ui::control *>::iterator interface::window_iterator() const {
		auto iter = std::find(swappo->begin(), swappo->end(), active_window);
		if (iter == swappo->end())
			throw std::runtime_error("The active window isn't a child of swappo");
		return iter;
	}

	void interface::update_titlebar(const std::shared_ptr<pingpong::channel> &chan) {
		if (!chan) {
			DBG("chan is null in update_titlebar");
			return;
		}

		titlebar->set_text(" " + std::string(chan->topic));
	}

	bool interface::can_remove(window *win) const {
		if (win == nullptr)
			win = active_window;

		return !(win->is_status() || win->is_overlay() || (win->is_channel() && !win->is_dead()));
	}

	std::string interface::colorize_if_dead(const std::string &str, window *win) const {
		return win->data.dead? ansi::red(str) : ansi::bold(str);
	}


// Public instance methods


	void interface::draw() {
		term->draw();
	}

	void interface::start() {
		term->watch_size();
	}

	void interface::log(const std::string &line, window *win) {
		log(haunted::ui::simpleline(line, 0), win);
	}

	void interface::log(const std::string &line, const std::string &window_name) {
		log(haunted::ui::simpleline(line, 0), window_name);
	}

	void interface::log(const std::exception &err) {
		log(ansi::red(haunted::util::demangle_object(err)) + ": "_d + ansi::bold(err.what()));
	}

	void interface::focus_window(window *win) {
		if (active_window == overlay) {
			before_overlay = nullptr;
			input->focus();
		}

		if (win == nullptr)
			win = status_window;

		if (win == active_window)
			return;

		swappo->set_active(active_window = win);

		if (win == overlay) {
			update_overlay();
			overlay->focus();
		}

		update_statusbar();
		update_titlebar();
	}

	void interface::focus_window(const std::string &window_name) {
		focus_window(get_window(window_name));
	}

	void interface::next_server() {
		if (active_window != status_window || parent->pp.servers.size() < 2)
			return;

		const std::set<pingpong::server *> &servers = parent->pp.servers;
		auto iter = servers.find(parent->active_server());
		if (++iter == servers.end())
			iter = servers.begin();
		parent->pp.active_server = *iter;
		log(lines::notice + "Switched to " + ansi::bold((*iter)->hostname) + ".");
		update_statusbar();
		update_titlebar();
	}

	void interface::next_window() {
#ifdef OVERLAY_PREVENTS_SWAPPING
		if (active_window == overlay)
			return;
#endif
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<window *>(swappo->get_children().at(0)));
		} else {
			auto iter = window_iterator();
			if (swappo->size() > 1 || *iter != overlay) {
				// If the only window is the overlay, then this loop would get stuck. This shouldn't be possible
				// (the status window should always be a child of swappo), but it can't hurt to prevent it anyway.
				do {
					if (++iter == swappo->end())
						iter = swappo->begin();
				} while (*iter == overlay); // Skip the overlay.
				focus_window(dynamic_cast<window *>(*iter));
			}
		}
	}

	void interface::previous_window() {
#ifdef OVERLAY_PREVENTS_SWAPPING
		if (active_window == overlay)
			return;
#endif
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<window *>(swappo->get_children().at(0)));
		} else {
			auto iter = window_iterator();
			if (swappo->size() > 1 || *iter != overlay) {
				do {
					if (iter == swappo->begin())
						iter = swappo->end();
					--iter;
				} while (*iter == overlay);
				focus_window(dynamic_cast<window *>(*iter));
			}
		}
	}

	void interface::update_statusbar() {
		window *win = active_window == overlay? before_overlay : active_window;

		if (!win) {

			statusbar->set_text("[?]");

		} else if (win->is_status()) {

			statusbar->set_text("[" + ansi::bold(win->window_name) + "] [" +
				ansi::bold(parent->active_server_name()) + "]");

		} else if (win->is_channel()) {

			statusbar->set_text("[" + ansi::bold(parent->active_server_name()) + "] [" +
				colorize_if_dead(win->data.chan->name, win) + "]");

		} else if (win->is_user()) {

			statusbar->set_text("[" + ansi::bold(parent->active_server_name()) + "] [" +
				colorize_if_dead(win->data.user->name, win) + "]");

		} else {
			statusbar->set_text("[" + ansi::bold(win->window_name) + "]");
		}
	}

	void interface::update_overlay() {
		overlay->clear_lines();

		if (before_overlay == nullptr)
			return;

		if (before_overlay == status_window) {
			*overlay += haunted::ui::simpleline(ansi::bold("Servers"));
			for (pingpong::server *serv: parent->pp.servers) {
				serv->sort_channels();
				if (serv->is_active()) {
					*overlay += lines::status_server_line(serv);
					for (std::shared_ptr<pingpong::channel> chan: serv->channels)
						*overlay += lines::status_channel_line(chan);
				}
			}

			return;
		}

		const window_meta &data = before_overlay->data;

		window_type type = data.type;
		if (type == window_type::channel)
			update_overlay(data.chan);
		else if (type == window_type::user)
			update_overlay(data.user);

		overlay->draw();
	}

	void interface::update_titlebar() {
		window_type type = active_window->data.type;
		if (type == window_type::channel) {
			update_titlebar(active_window->data.chan);
		} else {
			titlebar->clear();
		}
	}

	window * interface::toggle_overlay() {
		if (active_window == overlay) {
			window *old_active = active_window;
			focus_window(before_overlay);
			return old_active;
		}

		before_overlay = active_window;
		focus_window(overlay);
		return before_overlay;
	}

	std::vector<window *> interface::windows_for_user(std::shared_ptr<pingpong::user> user) const {
		if (swappo->empty())
			return {};

		std::vector<window *> found {};

		for (haunted::ui::control *ctrl: swappo->get_children()) {
			window *win = dynamic_cast<window *>(ctrl);
			if (!win)
				continue;

			if ((win->is_user() && *user == *win->data.user) || (win->is_channel() && win->data.chan->has_user(user)))
				found.push_back(win);
		}

		return found;
	}

	window * interface::window_for_channel(std::shared_ptr<pingpong::channel> chan) const {
		for (haunted::ui::control *ctrl: swappo->get_children()) {
			window *win = dynamic_cast<window *>(ctrl);
			if (win && win->data.chan == chan)
				return win;
		}

		return nullptr;
	}

	std::shared_ptr<pingpong::channel> interface::get_active_channel() const {
		window *win = before_overlay? before_overlay : active_window;
		if (win->is_channel())
			return win->data.chan;
		return nullptr;
	}

	std::shared_ptr<pingpong::user> interface::get_active_user() const {
		if (active_window->is_user())
			return active_window->data.user;
		return nullptr;
	}

	bool interface::is_active(window *win) const {
		if (!win)
			return false;

		return active_window == win || before_overlay == win;
	}

	bool interface::on_key(const haunted::key &k) {
		if (k == config::keys::toggle_overlay) {
			toggle_overlay();
		} else if (k == config::keys::switch_server) {
			next_server();
		} else if (k == config::keys::next_window) {
			next_window();
		} else if (k == config::keys::previous_window) {
			previous_window();
		} else if (k == haunted::kmod::ctrl) {
			switch (k.type) {
				case haunted::ktype::g: log("Active server: " + parent->active_server_name()); break;
				case haunted::ktype::r:
					if (active_window)
						DBG("v = " << active_window->get_voffset() << ", e = " << active_window->effective_voffset());
					break;
				default: return false;
			}

			return true;
		} else if (k == haunted::kmod::shift) {
			if (!active_window)
				return false;
			
			switch (k.type) {
				case haunted::ktype::up_arrow: active_window->vscroll(-1); break;
				case haunted::ktype::down_arrow: {
					active_window->vscroll(1); 
					const int total = active_window->total_rows(), height = active_window->get_position().height,
					          effective = active_window->effective_voffset();
					if (total == height + effective)
						active_window->set_voffset(-1);
					break;
				}
				case haunted::ktype::left_arrow: active_window->set_voffset(-1); break;
				default: return false;
			}

			return true;
		}

		return false;
	}
}
