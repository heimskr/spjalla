#ifndef SPJALLA_UI_INTERFACE_H_
#define SPJALLA_UI_INTERFACE_H_

#include <deque>
#include <memory>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "haunted/core/hdefs.h"
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
	class client;
}

namespace spjalla::ui {
	class interface {
		friend class spjalla::client;

		private:
			haunted::terminal *term;
			client *parent;

			std::list<window *> windows;
			window *status_window, *active_window;
			
			window *overlay;

			/** Whenever the overlay is activated, a pointer to the previous window is kept in this variable so that the
			 *  correct window can be restored when the overlay is closed. */
			window *before_overlay = nullptr;

			haunted::ui::boxes::swapbox    *swappo;
			haunted::ui::boxes::expandobox *expando;
			haunted::ui::label     *titlebar, *statusbar;
			haunted::ui::textinput *input;

			/** Sets up the labels, overlay and textinput. */
			void init_basic();

			/** Sets up the swapbox that contains all the windows. */
			void init_swappo();

			/** Sets up the expandobox that serves as the program's root control. */
			void init_expando();

			/** Colors all the controls. */
			void init_colors();

			/** Returns a pointer to the window indicated by a given string. If no window is found, one will be created
			 *  with that name if `create` is true. */
			window * get_window(const std::string &, bool create = false, window_type = window_type::other);

			/** Returns a pointer to the window corresponding to a given channel. If no window is found, one will be
			 *  created for the channel if `create` is true. */
			window * get_window(const std::shared_ptr<pingpong::channel> &, bool create = false);

			/** Returns a pointer to the window corresponding to a given user. If no window is found, one will be
			 *  created for the user if `create` is true. */
			window * get_window(const std::shared_ptr<pingpong::user> &, bool create = false);

			/** Creates a new window, configures it as appropriate and appends it to the swapbox. */
			window * new_window(const std::string &name, window_type);

			/** Closes a window. */
			void remove_window(window *);

			/** Renders a channel's user list onto the overlay. */
			void update_overlay(const std::shared_ptr<pingpong::channel> &);

			/** Renders a user's channel list onto the overlay. */
			void update_overlay(const std::shared_ptr<pingpong::user> &);

			/** Renders a channel's topic onto the titlebar. */
			void update_titlebar(const std::shared_ptr<pingpong::channel> &);

			/** Returns the iterator pointing to the active window in swappo's children. */
			std::vector<haunted::ui::control *>::iterator window_iterator() const;

			/** Returns whether it's okay to immediately remove a given window. */
			bool can_remove(window * = nullptr) const;

			/** Styles a window based on whether it's dead. */
			std::string colorize_if_dead(const std::string &, window *) const;

		public:
			interface(haunted::terminal *, client * = nullptr);

			/** Redraws the interface. */
			void draw();

			/** Starts any workers (or the like) needed for this interface. */
			void start();

			/** Logs a line of output for a given target window. */
			template <typename T>
			void log(const T &line, window *win = nullptr) {
				if (win == nullptr)
					win = status_window;
				*win += line;
			}

			/** Logs a line of output for a given target window. */
			void log(const std::string &, window * = nullptr);

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			template <typename T>
			void log(const T &line, const std::string &window_name) {
				log(line, get_window(window_name));
			}

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			void log(const std::string &line, const std::string &window_name);

			/** Logs an exception to the status window. */
			void log(const std::exception &);

			/** Focuses a window. Note that this method will swap the active window and the given window, so the pointer
			 *  given will point to a different window after the method is called (assuming the given window isn't
			 *  already the active window, in which case nothing would happen). */
			void focus_window(window * = nullptr);
			void focus_window(const std::string &);

			/** Switches to the next server in the list. */
			void next_server();

			/** Switches to the next window after the current window. */
			void next_window();

			/** Switches to the previous window before the current window. */
			void previous_window();

			/** Updates the text in the status bar. */
			void update_statusbar();

			/** Updates the text in the overlay. */
			void update_overlay();

			/** Updates the text in the titlebar. */
			void update_titlebar();

			/** Toggles the overlay and returns a pointer to the window that was covered/uncovered. */
			window * toggle_overlay();

			/** Returns all windows (for channels or private conversations) where a given user is present. */
			std::vector<window *> windows_for_user(std::shared_ptr<pingpong::user>) const;

			/** Returns the window for a given channel. */
			window * window_for_channel(std::shared_ptr<pingpong::channel>) const;

			/** Returns the active window. ¯\_(ツ)_/¯ */
			window * get_active_window() { return active_window; }

			/** If the active window (or the window below the overlay, if the overlay is active) is for a channel,
			 *  this returns the pointer to the relevant channel. */
			std::shared_ptr<pingpong::channel> get_active_channel() const;

			/** If the active window is for a user, this returns the pointer to the relevant user. */
			std::shared_ptr<pingpong::user> get_active_user() const;

			/** Returns true if the window is active or currently covered by the overlay. */
			bool is_active(window *) const;

			/** Handles keypresses that aren't handled by the textinput. */
			bool on_key(const haunted::key &);
	};
}

#endif
