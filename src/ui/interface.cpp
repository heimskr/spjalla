#include <iomanip>
#include <memory>
#include <thread>

#include <csignal>

#include "ui/interface.h"
#include "haunted/core/defs.h"
#include "haunted/core/key.h"

namespace spjalla {
	interface::interface(haunted::terminal *term): term(term) {
		using haunted::ui::boxes::box_orientation;
		using namespace haunted::ui;
		using namespace spjalla::ui;

		input     = new textinput(term);
		userbox   = new textbox(term);
		titlebar  = new label(term);
		statusbar = new label(term);

		input->set_name("input");
		userbox->set_name("userbox");
		titlebar->set_name("titlebar");
		statusbar->set_name("statusbar");

		status_window = new window("status");
		status_window->set_terminal(term);
		status_window->set_name("status_window");
		active_window = status_window;
		windows.push_front(status_window);

		swappo = new boxes::swapbox(term, {}, {active_window});

		control *first, *second;
		if (users_side == haunted::side::left) {
			first = userbox;
			second = swappo;
		} else {
			first = swappo;
			second = userbox;
		}

		propo = new boxes::propobox(term, adjusted_ratio(), box_orientation::horizontal, first, second);
		swappo->set_parent(propo);

		expando = new boxes::expandobox(term, term->get_position(), box_orientation::vertical,
			{{titlebar, 1}, {propo, -1}, {statusbar, 1}, {input, 1}});

		propo->set_name("propo");
		expando->set_name("expando");
		expando->resize();

		userbox->set_colors(ansi::color::green, ansi::color::red);
		input->set_colors(ansi::color::magenta, ansi::color::yellow);
		titlebar->set_colors(ansi::color::blue, ansi::color::orange);
		statusbar->set_colors(ansi::color::orange, ansi::color::blue);
		active_window->set_colors(ansi::color::red, ansi::color::green);

		input->focus();

		term->set_root(expando);

		DBG("term == " << term);
		DBG("expando == " << expando);
		DBG("propo == " << propo);
		DBG("input == " << input);
		DBG("active_window == " << active_window);
		DBG("userbox == " << userbox);
		DBG("titlebar == " << titlebar);
		DBG("statusbar == " << statusbar);
	}


// Private instance methods


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
		return users_side == haunted::side::right? 1 - users_ratio : users_ratio;
	}

	ui::window * interface::get_window(const std::string &window_name, bool create) {
		if (window_name == "status") {
			if (status_window == nullptr && create) {
				status_window = new ui::window(swappo, swappo->get_position(), "status");
			}

			return status_window;
		}

		if (swappo->empty())
			return nullptr;

		throw std::runtime_error("Unimplemented.");
	}

	size_t interface::get_output_index() const {
		return users_side == haunted::side::left? 1 : 0;
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

	void interface::log(const haunted::ui::textline &, ui::window *win) {
		if (win == nullptr)
			win = status_window;
	}

	void interface::log(const haunted::ui::textline &line, const std::string &window_name) {
		log(line, get_window(window_name));
	}

	void interface::focus_window(ui::window *win) {
		if (win == nullptr)
			win = status_window;

		if (win == active_window)
			return;

		active_window->set_parent(nullptr);
		win->set_parent(propo);

		std::swap(*active_window, *win);
	}

	void interface::focus_window(const std::string &window_name) {
		focus_window(get_window(window_name));
	}
}
