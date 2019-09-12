#include <iomanip>
#include <memory>
#include <thread>

#include <csignal>

#include "ui/interface.h"
#include "lib/haunted/core/defs.h"
#include "lib/haunted/core/key.h"
#include "lib/pingpong/core/channel.h"

namespace spjalla {
	interface::interface(haunted::terminal *term): term(term) {
		init_basic();
		init_swappo();
		init_propo();
		init_expando();
		init_colors();

		input->focus();
		update_statusbar();
	}


// Private instance methods


	void interface::init_basic() {
		input = new haunted::ui::textinput(term);
		input->set_name("input");
	
		userbox = new haunted::ui::textbox(term);
		userbox->set_name("userbox");
	
		titlebar = new haunted::ui::label(term);
		titlebar->set_name("titlebar");

		statusbar = new haunted::ui::label(term);
		statusbar->set_name("statusbar");
	}

	void interface::init_swappo() {
		status_window = new ui::window("status");
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
		if (users_side == haunted::side::left) {
			first = userbox;
			second = swappo;
		} else {
			first = swappo;
			second = userbox;
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
		// userbox->set_colors(ansi::color::green, ansi::color::red);
		// input->set_colors(ansi::color::magenta, ansi::color::yellow);
		// titlebar->set_colors(ansi::color::blue, ansi::color::orange);
		statusbar->set_colors(ansi::color::white, ansi::color::blue);
		active_window->set_colors(ansi::color::normal, ansi::color::normal);
	}

	void interface::readjust_columns() {
		bool changed = false;
		std::vector<haunted::ui::control *> &pchildren = propo->get_children();

		if (pchildren[get_output_index()] == userbox) {
			std::swap(pchildren.at(0), pchildren.at(1));
			// haunted::ui::control *back = pchildren.back();
			// pchildren.pop_back();

			// std::swap(*active_window, *userbox);
			changed = true;
		}

		double adjusted = adjusted_ratio();

		if (propo->get_ratio() != adjusted) {
			propo->set_ratio(adjusted);
		} else if (changed) {
			// set_ratio() already draws if the ratio changed (and that's true for the preceding if block).
			propo->draw();
		}
	}

	double interface::adjusted_ratio() const {
		// It's best to avoid division by zero.
		if (users_side == haunted::side::right && users_ratio == 0.0)
			return 0.0;

		return users_side == haunted::side::right? 1.0 / users_ratio : users_ratio;
	}

	ui::window * interface::get_window(const std::string &window_name, bool create) {
		if (window_name == "status") {
			if (status_window == nullptr && create)
				status_window = new_window("status");
			return status_window;
		}

		if (swappo->empty())
			return nullptr;

		for (haunted::ui::control *ctrl: swappo->get_children()) {
			ui::window *win = dynamic_cast<ui::window *>(ctrl);
			if (win->window_name == window_name)
				return win;
		}

		return new_window(window_name);
	}

	ui::window * interface::get_window(pingpong::channel_ptr chan, bool create) {
		return get_window(chan->serv->hostname + " " + chan->name, create);
	}

	ui::window * interface::new_window(const std::string &name) {
		static size_t win_count = 0;
		ui::window *win = new ui::window(swappo, swappo->get_position(), name);
		win->set_name("window" + std::to_string(++win_count));
		win->set_voffset(-1);
		return win;
	}

	size_t interface::get_output_index() const {
		return users_side == haunted::side::left? 1 : 0;
	}

	void interface::update_statusbar() {
		if (!active_window) {
			statusbar->set_text("[?]"_d);
		} else {
			statusbar->set_text("["_d + active_window->window_name + "]"_d);
		}
	}


// Public instance methods


	void interface::set_users_side(haunted::side side) {
		if (users_side != side) {
			users_side = side;
			readjust_columns();
		}
	}

	void interface::set_users_ratio(double ratio) {
		if (users_ratio != ratio) {
			users_ratio = ratio;
			readjust_columns();
		}
	}

	void interface::draw() {
		term->draw();
	}

	void interface::start() {
		term->watch_size();
	}

	void interface::log(const std::string &line, ui::window *win) {
		log(haunted::ui::simpleline(line, 0), win);
	}

	void interface::log(const std::string &line, const std::string &window_name) {
		log(haunted::ui::simpleline(line, 0), window_name);
	}

	void interface::focus_window(ui::window *win) {
		if (win == nullptr)
			win = status_window;

		if (win == active_window)
			return;

		swappo->set_active(active_window = win);
		swappo->draw();
		update_statusbar();
	}

	void interface::focus_window(const std::string &window_name) {
		focus_window(get_window(window_name));
	}

	void interface::next_window() {
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<ui::window *>(swappo->get_children().at(0)));
		} else {
			auto iter = std::find(swappo->begin(), swappo->end(), active_window);
			haunted::ui::control *win = *(++iter == swappo->end()? swappo->begin() : iter);
			focus_window(dynamic_cast<ui::window *>(win));
		}
	}

	void interface::prev_window() {
		if (swappo->empty()) {
			active_window = nullptr;
		} else if (!active_window) {
			focus_window(dynamic_cast<ui::window *>(swappo->get_children().at(0)));
		} else {
			auto iter = std::find(swappo->begin(), swappo->end(), active_window);
			if (iter == swappo->begin())
				iter = swappo->end();
			--iter;
			focus_window(dynamic_cast<ui::window *>(*iter));
		}
	}

	bool interface::on_key(const haunted::key &k) {
		if (k == haunted::kmod::ctrl) {
			switch (k.type) {
				case haunted::ktype::n: next_window(); break;
				case haunted::ktype::p: prev_window(); break;
				default: return false;
			}

			return true;
		}

		return false;
	}
}
