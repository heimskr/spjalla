#ifndef UI_INTERFACE_H_
#define UI_INTERFACE_H_

#include <deque>
#include <memory>
#include <iostream>
#include <sstream>
#include <thread>

#include "lib/haunted/core/defs.h"
#include "lib/haunted/core/terminal.h"
#include "lib/haunted/ui/label.h"
#include "lib/haunted/ui/textbox.h"
#include "lib/haunted/ui/textinput.h"
#include "lib/haunted/ui/boxes/expandobox.h"
#include "lib/haunted/ui/boxes/propobox.h"

namespace spjalla {
	class interface {
		friend class client;

		private:
			haunted::terminal *term;
			std::shared_ptr<std::thread> worker_draw, worker_input;

			haunted::ui::boxes::propobox *propo;
			haunted::ui::boxes::expandobox *expando;
			haunted::ui::label *titlebar, *statusbar;
			haunted::ui::textbox *output, *userbox;
			haunted::ui::textinput *input;

			void work_draw();
			void work_input();
			void process_input();
			void render_input();

			/** Sets the propobox's ratio based on users_width and users_side. */
			void readjust_columns();

			/** Returns the ratio appropriate for use in the propobox (it depends on users_side). */
			double adjusted_ratio() const;

		public:
			haunted::side users_side = haunted::side::left;
			double users_ratio = 0.2;
			size_t max_lines = 128;

			interface(haunted::terminal *term);
			~interface();

			void set_users_side(haunted::side);
			void set_users_ratio(double);

			void draw();
			void start();
			void join();
	};
}

#endif
