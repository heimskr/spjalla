#include <iomanip>
#include <memory>
#include <thread>

#include <csignal>

#include "ui/interface.h"
#include "lib/haunted/core/hdefs.h"
#include "lib/haunted/core/key.h"
#include "lib/pingpong/core/ppdefs.h"
#include "lib/pingpong/core/channel.h"
#include "lib/pingpong/core/user.h"
#include "lines/userlist.h"

#include "core/client.h"

namespace spjalla::ui {
	interface::interface(haunted::terminal *term_, client *parent_): term(term_), parent(parent_) {
		init_basic();
		init_swappo();
		init_propo();
		init_expando();
		init_colors();

		input->focus();
		update_statusbar();
		update_sidebar();
	}


// Private instance methods


	void interface::init_basic() {
		input = new haunted::ui::textinput(term);
		input->set_name("input");
	
		sidebar = new haunted::ui::textbox(term);
		sidebar->set_name("sidebar");
	
		titlebar = new haunted::ui::label(term);
		titlebar->set_name("titlebar");

		statusbar = new haunted::ui::label(term);
		statusbar->set_name("statusbar");
	}

	void interface::init_swappo() {
		status_window = new window("status");
		status_window->data = {window_type::status};
		status_window->set_terminal(term);
		status_window->set_name("status_window");
		status_window->set_voffset(-1);
		active_window = status_window;
		windows.push_front(status_window);

		swappo = new haunted::ui::boxes::swapbox(term, {}, {active_window});
		swappo->set_name("swappo");
	}

	void interface::init_propo() {
		haunted::ui::control *first, *second;
		if (sidebar_side == haunted::side::left) {
			first = sidebar;
			second = swappo;
		} else {
			first = swappo;
			second = sidebar;
		}

		propo = new haunted::ui::boxes::propobox(term, adjusted_ratio(),
			haunted::ui::boxes::box_orientation::horizontal, first, second);
		swappo->set_parent(propo);
		propo->set_name("propo");
	}

	void interface::init_expando() {
		expando = new haunted::ui::boxes::expandobox(term, term->get_position(),
			haunted::ui::boxes::box_orientation::vertical, {{titlebar, 1}, {propo, -1}, {statusbar, 1}, {input, 1}});
		expando->set_name("expando");
		term->set_root(expando);
		expando->key_fn = [&](const haunted::key &k) { return on_key(k); };
	}

	void interface::init_colors() {
		sidebar->set_colors(ansi::color::normal, ansi::color::verydark);
		titlebar->set_colors(ansi::color::white, ansi::color::blue);
		statusbar->set_colors(ansi::color::white, ansi::color::blue);
		// input->set_colors(ansi::color::normal, ansi::color::red);
		// active_window->set_colors(ansi::color::normal, ansi::color::magenta);
	}

	void interface::readjust_columns() {
		bool changed = false;
		std::vector<haunted::ui::control *> &pchildren = propo->get_children();

		if (pchildren[get_output_index()] == sidebar) {
			std::swap(pchildren.at(0), pchildren.at(1));
			changed = true;
		}

		double adjusted = adjusted_ratio();

		if (propo->get_ratio() != adjusted) {
			propo->set_ratio(adjusted);
		} else if (changed) {
			// set_ratio() already resizes if the ratio changed (and that's true for the preceding if block).
			propo->resize();
		}
	}

	double interface::adjusted_ratio() const {
		// It's best to avoid division by zero.
		if (sidebar_side == haunted::side::right && sidebar_ratio == 0.0)
			return 0.0;

		return sidebar_side == haunted::side::right? 1.0 / sidebar_ratio : sidebar_ratio;
	}

	window * interface::get_window(const std::string &window_name, bool create) {
		if (window_name == "status") {
			if (status_window == nullptr && create)
				status_window = new_window("status");
			return status_window;
		}

		if (swappo->empty())
			return nullptr;

		for (haunted::ui::control *ctrl: swappo->get_children()) {
			window *win = dynamic_cast<window *>(ctrl);
			if (win->window_name == window_name)
				return win;
		}

		return create? new_window(window_name) : nullptr;
	}

	window * interface::get_window(pingpong::channel_ptr chan, bool create) {
		const std::string name = chan->serv->hostname + "/" + chan->name;
		window *win = get_window(name, false);

		if (create && !win) {
			win = new_window(name);
			win->data = {window_type::channel};
			win->data->chan = chan;
			win->data->serv = chan->serv;
		}

		return win;
	}

	window * interface::new_window(const std::string &name) {
		static size_t win_count = 0;
		window *win = new window(swappo, swappo->get_position(), name);
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

	size_t interface::get_output_index() const {
		return sidebar_side == haunted::side::left? 1 : 0;
	}

	void interface::update_sidebar(pingpong::channel_ptr chan) {
		*sidebar += haunted::ui::simpleline(ansi::bold(chan->name));
		chan->users.sort([&](pingpong::user_ptr left, pingpong::user_ptr right) -> bool {
			return left->name < right->name;
		});

		for (pingpong::user_ptr user: chan->users)
			*sidebar += spjalla::lines::userlist_line(chan, user);
	}



// Public instance methods


	void interface::set_sidebar_side(haunted::side side) {
		if (sidebar_side != side) {
			sidebar_side = side;
			readjust_columns();
		}
	}

	void interface::set_sidebar_ratio(double ratio) {
		if (sidebar_ratio != ratio) {
			sidebar_ratio = ratio;
			readjust_columns();
		}
	}

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

	void interface::focus_window(window *win) {
		if (win == nullptr)
			win = status_window;

		if (win == active_window)
			return;

		swappo->set_active(active_window = win);
		swappo->draw();
		update_statusbar();
		update_sidebar();
	}

	void interface::focus_window(const std::string &window_name) {
		focus_window(get_window(window_name));
	}

	void interface::next_window() {
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<window *>(swappo->get_children().at(0)));
		} else {
			auto iter = std::find(swappo->begin(), swappo->end(), active_window);
			haunted::ui::control *win = *(++iter == swappo->end()? swappo->begin() : iter);
			focus_window(dynamic_cast<window *>(win));
		}
	}

	void interface::prev_window() {
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<window *>(swappo->get_children().at(0)));
		} else {
			auto iter = std::find(swappo->begin(), swappo->end(), active_window);
			if (iter == swappo->begin())
				iter = swappo->end();
			--iter;
			focus_window(dynamic_cast<window *>(*iter));
		}
	}

	void interface::update_statusbar() {
		if (!active_window) {
			statusbar->set_text("[?]");
		} else {
			statusbar->set_text("[" + ansi::wrap(active_window->window_name, ansi::style::bold) + "]");
		}
	}

	void interface::update_sidebar() {
		sidebar->clear_lines();

		if (active_window == status_window) {
			*sidebar += haunted::ui::simpleline(ansi::bold("Servers"));
			for (pingpong::server_ptr serv: parent->pp.servers) {
				using pingpong::server;
				if (serv->status != server::stage::dead && serv->status != server::stage::unconnected)
					*sidebar += haunted::ui::simpleline(ansi::dim("- ") + std::string(*serv));
			}
			return;
		}

		std::optional<window_meta> &data = active_window->data;

		if (!data) {
			DBG("No data for " << active_window->window_name);
			return;
		}

		window_type type = data->type;
		if (type == window_type::channel) {
			update_sidebar(data->chan);
		}

		sidebar->draw();
	}

	std::vector<window *> interface::windows_for_user(pingpong::user_ptr user) const {
		if (swappo->empty())
			return {};

		std::vector<window *> found {};

		for (haunted::ui::control *ctrl: swappo->get_children()) {
			window *win = dynamic_cast<window *>(ctrl);
			if (!win->data)
				continue;

			const window_meta &data = *win->data;
			window_type type = data.type;

			if ((type == window_type::user && *user == *data.user) ||
				(type == window_type::channel && data.chan->has_user(user))) {
				found.push_back(win);
			}
		}

		return found;
	}

	pingpong::channel_ptr interface::get_active_channel() const {
		if (active_window && active_window->data && active_window->data->type == window_type::channel)
			return active_window->data->chan;
		return nullptr;
	}

	bool interface::on_key(const haunted::key &k) {
		if (k == haunted::kmod::ctrl) {
			switch (k.type) {
				case haunted::ktype::n: next_window(); break;
				case haunted::ktype::p: prev_window(); break;
				case haunted::ktype::h: sidebar->draw(); break;
				case haunted::ktype::g: active_window->draw(); break;
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
				case haunted::ktype::up_arrow:   active_window->vscroll(-1); break;
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
