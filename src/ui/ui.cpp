#include <iomanip>
#include <memory>
#include <thread>

#include <csignal>

#include "ui/ui.h"
#include "haunted/core/key.h"

namespace spjalla {
	// ui::ui(haunted::terminal *term): term(term), expando({term, term == nullptr? {} : term->get_position(), haunted::ui::boxes::box_orientation::vertical}) {
	ui::ui(haunted::terminal *term): term(term) {
	// propo(haunted::ui::boxes::propobox(term, 0.5, &output, &input)),
	// expando(haunted::ui::boxes::expandobox(term, term == nullptr? haunted::position() : term->get_position(), haunted::ui::boxes::box_orientation::vertical)),
	// titlebar(haunted::ui::label(term)),
	// statusbar(haunted::ui::label(term)),
	// output(haunted::ui::textbox(term)),
	// userbox(haunted::ui::textbox(term)),
	// input(haunted::ui::textinput(term))
		using haunted::ui::boxes::box_orientation;

		input     = new haunted::ui::textinput(term);
		userbox   = new haunted::ui::textbox(term);
		output    = new haunted::ui::textbox(term);
		titlebar  = new haunted::ui::label(term);
		statusbar = new haunted::ui::label(term);
		haunted::ui::textbox *first, *second;
		std::tie(first, second) = users_side == haunted::side::left? std::pair(userbox, output) : std::pair(output, userbox);
		propo     = new haunted::ui::boxes::propobox(term, adjusted_ratio(), first, second);
		expando   = new haunted::ui::boxes::expandobox(term, term->get_position(), box_orientation::vertical, {
			{titlebar, 1}, {propo, -1}, {statusbar, 1}, {input, 1}
		});
	}

	void ui::readjust_columns() {
		bool changed = false;
		if (propo->get_children()[users_side == haunted::side::left? 1 : 0] == userbox) {
			std::swap(*output, *userbox);
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

	double ui::adjusted_ratio() const {
		return users_side == haunted::side::right? 1 - users_ratio : users_ratio;
	}

	void ui::set_users_side(haunted::side side) {
		if (users_side != side) {
			users_side = side;
			readjust_columns();
		}
	}

	void ui::set_users_ratio(double ratio) {
		if (users_ratio != ratio) {
			users_ratio = ratio;
			readjust_columns();
		}
	}

	void ui::draw() {
	}

	void ui::start() {
		term->watch_size();
		worker_draw  = std::make_shared<std::thread>(&ui::work_draw,  this);
		worker_input = std::make_shared<std::thread>(&ui::work_input, this);
	}

	void ui::work_draw() {
		draw();
	}

	void ui::work_input() {
		haunted::key k;
		// while
	}

	void ui::render_input() {

	}

	void ui::process_input() {
		std::cout << "\r\e[2KString: \"" << input << "\" [" << input->length() << "]\r\n\e[2K\e[2G";
		input->clear();
	}

	void ui::join() {
		worker_draw->join();
		worker_input->join();
	}
}
