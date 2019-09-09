#ifndef UI_INTERFACE_H_
#define UI_INTERFACE_H_

#include <deque>
#include <memory>
#include <iostream>
#include <list>
#include <sstream>
#include <thread>

#include "lib/haunted/core/defs.h"
#include "lib/haunted/core/terminal.h"
#include "lib/haunted/ui/label.h"
#include "lib/haunted/ui/textbox.h"
#include "lib/haunted/ui/textinput.h"
#include "lib/haunted/ui/boxes/expandobox.h"
#include "lib/haunted/ui/boxes/propobox.h"
#include "lib/haunted/ui/boxes/swapbox.h"

#include "ui/window.h"

namespace spjalla {
	class interface {
		friend class client;

		private:
			haunted::terminal *term;
			std::shared_ptr<std::thread> worker_draw, worker_input;

			std::list<ui::window *> windows;
			ui::window *status_window;
			ui::window *active_window;

			haunted::ui::boxes::swapbox    *swappo;
			haunted::ui::boxes::propobox   *propo;
			haunted::ui::boxes::expandobox *expando;
			haunted::ui::label     *titlebar, *statusbar;
			haunted::ui::textbox   *userbox;
			haunted::ui::textinput *input;

			void process_input();

			/** Sets the propobox's ratio based on users_width and users_side. */
			void readjust_columns();

			/** Returns the ratio appropriate for use in the propobox (it depends on users_side). */
			double adjusted_ratio() const;

			/** Returns a pointer to the window indicated by a given string. If no window is found, one will be created
			 *  with that name if `create` is true. */
			ui::window * get_window(const std::string &, bool create = false);

			/** Returns the index within the propobox's children vector in which the output window resides. */
			size_t get_output_index() const;

		public:
			haunted::side users_side = haunted::side::left;
			double users_ratio = 0.2;
			size_t max_lines = 128;

			interface(haunted::terminal *term);

			void set_users_side(haunted::side);
			void set_users_ratio(double);

			void draw();
			void start();

			/** Logs a line of output for a given target window. */
			void log(const haunted::ui::textline &, ui::window * = nullptr);

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			void log(const haunted::ui::textline &, const std::string &);

			/** Focuses a window. Note that this method will swap the active window and the given window, so the pointer
			 *  given will point to a different window after the method is called (assuming the given window isn't
			 *  already the active window, in which case nothing would happen). */
			void focus_window(ui::window * = nullptr);
			void focus_window(const std::string &);

	};
}

#endif
