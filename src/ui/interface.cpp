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

		input     = new haunted::ui::textinput(term);
		userbox   = new haunted::ui::textbox(term);
		output    = new haunted::ui::textbox(term);
		titlebar  = new haunted::ui::label(term);
		statusbar = new haunted::ui::label(term);

		input->set_name("input");
		userbox->set_name("userbox");
		output->set_name("output");
		titlebar->set_name("titlebar");
		statusbar->set_name("statusbar");

		haunted::ui::textbox *first, *second;
		std::tie(first, second) = users_side == haunted::side::left?
			std::pair(userbox, output) : std::pair(output, userbox);

		propo   = new haunted::ui::boxes::propobox(term, adjusted_ratio(), box_orientation::horizontal, first, second);
		propo->set_name("propo");
		expando = new haunted::ui::boxes::expandobox(term, term->get_position(), box_orientation::vertical, {
			{titlebar, 1}, {propo, -1}, {statusbar, 1}, {input, 1}
		});

		expando->set_name("expando");
		expando->resize();

		input->focus();
	}

	interface::~interface() {
		delete input;
		delete userbox;
		delete output;
		delete titlebar;
		delete statusbar;
		delete propo;
		delete expando;
	}

	void interface::readjust_columns() {
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

	double interface::adjusted_ratio() const {
		return users_side == haunted::side::right? 1 - users_ratio : users_ratio;
	}

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

	void interface::process_input() {
		std::cout << "\r\e[2KString: \"" << input << "\" [" << input->length() << "]\r\n\e[2K\e[2G";
		input->clear();
	}
}
