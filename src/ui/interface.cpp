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

		DBG("-------textinput--------");
		input     = new haunted::ui::textinput(term);
		DBG("--------textbox---------");
		userbox   = new haunted::ui::textbox(term);
		DBG("--------textbox---------");
		output    = new haunted::ui::textbox(term);
		DBG("---------label----------");
		titlebar  = new haunted::ui::label(term);
		DBG("---------label----------");
		statusbar = new haunted::ui::label(term);

		input->set_name("input");
		userbox->set_name("userbox");
		output->set_name("output");
		titlebar->set_name("titlebar");
		statusbar->set_name("statusbar");

		haunted::ui::textbox *first, *second;
		std::tie(first, second) = users_side == haunted::side::left?
			std::pair(userbox, output) : std::pair(output, userbox);

		DBG("---------propo----------");
		propo   = new haunted::ui::boxes::propobox(term, adjusted_ratio(), box_orientation::horizontal, first, second);
		DBG("--------expando---------");
		expando = new haunted::ui::boxes::expandobox(term, term->get_position(), box_orientation::vertical, {
			{titlebar, 1}, {propo, -1}, {statusbar, 1}, {input, 1}
		});

		DBG("old expandoparent = " << expando->get_parent());
		expando->set_parent(term);

		DBG("term = " << term);
		DBG("propo = " << propo);
		DBG("expando = " << expando);
		expando->resize();

		propo->set_name("propo");
		expando->set_name("expando");
		input->focus();
	}

	interface::~interface() {
		DBG(ansi::wrap("interface::~interface()", ansi::color::red) << ": joining.");
		join();
		DBG(ansi::wrap("interface::~interface()", ansi::color::red) << ": joined.");
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
		worker_draw  = std::make_shared<std::thread>(&interface::work_draw,  this);
		worker_input = std::make_shared<std::thread>(&interface::work_input, this);
	}

	void interface::work_draw() {
		draw();
	}

	void interface::work_input() {
		// haunted::key k;
		// term->cbreak();
		// term->cbreak();
		// DBG("Hello");
		// while (*term >> k) {
		// 	DBG("key = " << k);

		// 	if (k == haunted::key(haunted::ktype::c).ctrl())
		// 		break;

		// 	term->send_key(k);
		// }
	}

	void interface::render_input() {

	}

	void interface::process_input() {
		std::cout << "\r\e[2KString: \"" << input << "\" [" << input->length() << "]\r\n\e[2K\e[2G";
		input->clear();
	}

	void interface::join() {
		if (worker_draw && worker_draw->joinable()) {
			DBG("Joining worker_draw.");
			worker_draw->join();
			DBG("Joined worker_draw.");
		}

		if (worker_input && worker_input->joinable()) {
			DBG("Joining worker_input.");
			worker_input->join();
			DBG("Joined worker_input.");
		}
	}
}
