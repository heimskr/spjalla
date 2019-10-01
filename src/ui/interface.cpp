#include <iomanip>
#include <set>

#include "haunted/core/defs.h"
#include "haunted/core/key.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"
#include "pingpong/events/event.h"

#include "spjalla/core/client.h"
#include "spjalla/core/options.h"
#include "spjalla/core/util.h"

#include "spjalla/config/config.h"

#include "spjalla/events/window_changed.h"
#include "spjalla/events/window_closed.h"

#include "spjalla/lines/chanlist.h"
#include "spjalla/lines/overlay.h"
#include "spjalla/lines/timed.h"
#include "spjalla/lines/userlist.h"

namespace spjalla::ui {
	interface::interface(haunted::terminal *term_, client *parent_): term(term_), parent(parent_) {
		init_basic();
		init_swappo();
		init_expando();

		input->focus();
		update_statusbar();
		update_overlay();
	}


// Private instance methods


	void interface::init_basic() {
		input = new haunted::ui::textinput(term);
		input->set_name("input");
		input->set_prefix(ansi::dim(">> "));
	
		titlebar = new haunted::ui::label(term);
		titlebar->set_name("titlebar");

		statusbar = new haunted::ui::label(term);
		statusbar->set_name("statusbar");
	}

	void interface::init_swappo() {
		overlay = new window("overlay");
		overlay->ignore_index = true;
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
		status_window->set_autoscroll(true);
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

		const std::string name = chan->serv->id + "/" + chan->name;
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

		const std::string name = user->serv->id + "/" + user->name;
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
		win->set_autoscroll(true);
		return win;
	}

	void interface::remove_window(window *win) {
		if (win->get_parent() != swappo)
			throw std::invalid_argument("Can't remove window: not a child of swappo.");

		if (active_window == win)
			focus_window(status_window);

		win->unnotify();
		pingpong::events::dispatch<events::window_closed_event>(win);
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

	haunted::ui::container::type::iterator interface::window_iterator() const {
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


// Public instance methods


	void interface::draw() {
		term->draw();
	}

	void interface::start() {
		term->watch_size();
	}

	void interface::log(const std::string &line, window *win) {
		log(lines::timed_line(line, 0), win);
	}

	void interface::log(const std::string &line, const std::string &window_name) {
		log(lines::timed_line(line, 0), window_name);
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

		ui::window *old_active = active_window;
		swappo->set_active(active_window = win);

		pingpong::events::dispatch<events::window_changed_event>(old_active, win);
		win->unnotify();

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

	bool interface::focus_window(size_t index) {
		for (haunted::ui::control *ctrl: swappo->get_children()) {
			ssize_t control_index = ctrl->get_index();
			if (0 <= control_index && static_cast<size_t>(control_index) == index) {
				focus_window(dynamic_cast<window *>(ctrl));
				return true;
			}
		}

		return false;
	}

	void interface::next_server() {
		if (active_window != status_window || parent->irc.servers.size() < 2)
			return;

		const auto &servers = parent->irc.server_order;
		auto iter = std::find(servers.begin(), servers.end(), parent->active_server());
		if (++iter == servers.end())
			iter = servers.begin();
		parent->irc.active_server = *iter;
		log(lines::notice + "Switched to " + ansi::bold((*iter)->id) + ".");
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

		if (update_statusbar_fn) {
			update_statusbar_fn(win);
		} else if (!win) {
			statusbar->set_text("[?]");
		} else if (win->is_status()) {
			statusbar->set_text("["_d + win->window_name + "] ["_d + parent->active_server_id() + "]"_d);
		} else if (win->is_channel()) {
			statusbar->set_text("["_d + parent->active_server_id() + "] ["_d
			                    + util::colorize_if_dead(win->data.chan->name, win) + "]"_d);
		} else if (win->is_user()) {
			statusbar->set_text("["_d + parent->active_server_id() + "] ["_d
			                    + util::colorize_if_dead(win->data.user->name, win) + "]"_d);
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
			for (pingpong::server *serv: parent->irc.server_order) {
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

	std::deque<window *> interface::windows_for_user(std::shared_ptr<pingpong::user> user) const {
		if (swappo->empty())
			return {};

		std::deque<window *> found {};

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

	bool interface::is_active(const window *win) const {
		if (!win)
			return false;

		return active_window == win || before_overlay == win;
	}

	bool interface::overlay_visible() const {
		return active_window == overlay;
	}

	void interface::scroll_page(bool up) {
		if (active_window == overlay)
			return;

		active_window->vscroll(std::max(1, active_window->get_position().height / 2) * (up? -1 : 1));
	}

	bool interface::on_key(const haunted::key &key) {
		haunted::key copy {key};
		if (!parent->before_key(copy))
			return false;

		if (copy == config::keys::toggle_overlay) {
			toggle_overlay();
		} else if (copy == config::keys::switch_server) {
			next_server();
		} else if (copy == config::keys::next_window) {
			next_window();
		} else if (copy == config::keys::previous_window) {
			previous_window();
		} else if (copy == haunted::kmod::ctrl) {
			switch (copy.type) {
				case haunted::ktype::g: log("Active server: " + parent->active_server_id()); break;
				case haunted::ktype::r:
					if (active_window)
						DBG("v == " << active_window->get_voffset() << ", autoscroll == "
							<< (active_window->get_autoscroll()? "true" : "false"));
					break;
				case haunted::ktype::m: {
					if (active_window->get_autoscroll())
						DBG("autoscroll: true -> false");
					else
						DBG("autoscroll: false -> true");
					active_window->set_autoscroll(!active_window->get_autoscroll());
					break;
				}
				default: return false;
			}

			return true;
		} else if (copy == haunted::kmod::shift) {
			if (!active_window)
				return false;
			
			switch (copy.type) {
				case haunted::ktype::up_arrow: active_window->vscroll(-1); break;
				case haunted::ktype::down_arrow: {
					active_window->vscroll(1); 
					const int total = active_window->total_rows(), height = active_window->get_position().height,
					          voffset = active_window->get_voffset();
					if (total == height + voffset)
						active_window->set_autoscroll(true);
					break;
				}
				case haunted::ktype::left_arrow: active_window->set_autoscroll(true); break;
				default: return false;
			}

			return true;
		} else if (copy == haunted::kmod::alt) {
			switch (copy.type) {
				case haunted::ktype::_1: focus_window(static_cast<size_t>(0)); return true;
				case haunted::ktype::_2: focus_window(1);  return true;
				case haunted::ktype::_3: focus_window(2);  return true;
				case haunted::ktype::_4: focus_window(3);  return true;
				case haunted::ktype::_5: focus_window(4);  return true;
				case haunted::ktype::_6: focus_window(5);  return true;
				case haunted::ktype::_7: focus_window(6);  return true;
				case haunted::ktype::_8: focus_window(7);  return true;
				case haunted::ktype::_9: focus_window(8);  return true;
				case haunted::ktype::_0: focus_window(9);  return true;
				case haunted::ktype::q:  focus_window(10); return true;
				case haunted::ktype::w:  focus_window(11); return true;
				case haunted::ktype::e:  focus_window(12); return true;
				case haunted::ktype::r:  focus_window(13); return true;
				case haunted::ktype::t:  focus_window(14); return true;
				case haunted::ktype::y:  focus_window(15); return true;
				case haunted::ktype::u:  focus_window(16); return true;
				case haunted::ktype::i:  focus_window(17); return true;
				case haunted::ktype::o:  focus_window(18); return true;
				case haunted::ktype::p:  focus_window(19); return true;
				default: return false;
			}
		} else if (copy.mods.none()) {
			switch (copy.type) {
				case haunted::ktype::page_down:
				case haunted::ktype::page_up:
					scroll_page(copy == haunted::ktype::page_up);
					return true;
				case haunted::ktype::tab:
					parent->tab_complete();
					return true;
				default:;
			}
		}

		return false;
	}

	void interface::set_input(const std::string &str) {
		input->set_text(str);
	}

	std::string interface::get_input() const {
		return input->get_text();
	}

	void interface::set_bar_foreground(ansi::color fg) {
		statusbar->set_foreground(fg);
		titlebar->set_foreground(fg);
		statusbar->draw();
		titlebar->draw();
	}

	void interface::set_bar_background(ansi::color bg) {
		statusbar->set_background(bg);
		titlebar->set_background(bg);
		statusbar->draw();
		titlebar->draw();
	}

	void interface::set_overlay_foreground(ansi::color fg) {
		overlay->set_foreground(fg);
		overlay->draw();
	}

	void interface::set_overlay_background(ansi::color bg) {
		overlay->set_background(bg);
		overlay->draw();
	}

	void interface::set_input_foreground(ansi::color fg) {
		input->set_foreground(fg);
		input->draw();
	}

	void interface::set_input_background(ansi::color bg) {
		input->set_background(bg);
		input->draw();
	}
}
