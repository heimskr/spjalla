#ifndef SPJALLA_UI_INTERFACE_H_
#define SPJALLA_UI_INTERFACE_H_

#include <deque>
#include <memory>
#include <iostream>
#include <list>
#include <sstream>
#include <thread>

#include "haunted/core/defs.h"
#include "haunted/core/key.h"
#include "haunted/core/terminal.h"
#include "haunted/ui/label.h"
#include "haunted/ui/textbox.h"
#include "haunted/ui/textinput.h"
#include "haunted/ui/boxes/expandobox.h"
#include "haunted/ui/boxes/propobox.h"
#include "haunted/ui/boxes/swapbox.h"
#include "pingpong/core/ppdefs.h"

#include "ui/window.h"

namespace spjalla {
	class interface {
		friend class client;

		private:
			haunted::terminal *term;
			std::shared_ptr<std::thread> worker_draw, worker_input;

			std::list<ui::window *> windows;
			ui::window *status_window, *active_window;

			haunted::ui::boxes::swapbox    *swappo;
			haunted::ui::boxes::propobox   *propo;
			haunted::ui::boxes::expandobox *expando;
			haunted::ui::label     *titlebar, *statusbar;
			haunted::ui::textbox   *sidebar;
			haunted::ui::textinput *input;

			/** Sets up the labels, sidebar and textinput. */
			void init_basic();

			/** Sets up the swapbox that contains all the windows. */
			void init_swappo();

			/** Sets up the propobox that contains the sidebar and the swapbox. */
			void init_propo();

			/** Sets up the expandobox that serves as the program's root control. */
			void init_expando();

			/** Colors all the controls. */
			void init_colors();

			/** Sets the propobox's ratio based on sidebar_ratio and sidebar_side. */
			void readjust_columns();

			/** Returns the ratio appropriate for use in the propobox (it depends on sidebar_side). */
			double adjusted_ratio() const;

			/** Returns a pointer to the window indicated by a given string. If no window is found, one will be created
			 *  with that name if `create` is true. */
			ui::window * get_window(const std::string &, bool create = false);

			/** Returns a pointer to the window corresponding to a given channel. If no window is found, one will be
			 *  created for the channel if `create` is true. */
			ui::window * get_window(pingpong::channel_ptr, bool create = false);

			/** Creates a new window, configures it as appropriate and appends it to the swapbox. */
			ui::window * new_window(const std::string &name);

			/** Returns the index within the propobox's children vector in which the output window resides. */
			size_t get_output_index() const;

			/** Updates the text in the status bar. */
			void update_statusbar();

		public:
			haunted::side sidebar_side = haunted::side::right;
			double sidebar_ratio = 0.2;
			size_t max_lines = 128;

			interface(haunted::terminal *term);

			/** Sets the side of the screen that the sidebar should be on. */
			void set_sidebar_side(haunted::side);

			/** Changes the propobox ratio of the sidebar. */
			void set_sidebar_ratio(double);

			/** Redraws the interface. */
			void draw();

			/** Starts any workers (or the like) needed for this interface. */
			void start();

			/** Logs a line of output for a given target window. */
			template <typename T>
			void log(const T &line, ui::window *win = nullptr) {
				if (win == nullptr)
					win = status_window;
				*win += line;
			}

			/** Logs a line of output for a given target window. */
			void log(const std::string &, ui::window * = nullptr);

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			template <typename T>
			void log(const T &line, const std::string &window_name) {
				log(line, get_window(window_name));
			}

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			void log(const std::string &, const std::string &);

			/** Focuses a window. Note that this method will swap the active window and the given window, so the pointer
			 *  given will point to a different window after the method is called (assuming the given window isn't
			 *  already the active window, in which case nothing would happen). */
			void focus_window(ui::window * = nullptr);
			void focus_window(const std::string &);

			/** Switches to the next window after the current window. */
			void next_window();

			/** Switches to the previous window before the current window. */
			void prev_window();

			/** Returns all windows (for channels or private conversations) where a given user is present. */
			std::vector<ui::window *> windows_for_user(pingpong::user_ptr) const;

			/** Returns the active window. ¯\_(ツ)_/¯ */
			ui::window * get_active_window() { return active_window; }

			/** If the active window is for a channel, this returns the pointer to the relevant channel. */
			pingpong::channel_ptr get_active_channel() const;

			/** Handles keypresses that aren't handled by the textinput. */
			bool on_key(const haunted::key &);
	};
}

#endif
