#ifndef SPJALLA_UI_INTERFACE_H_
#define SPJALLA_UI_INTERFACE_H_

#include <deque>
#include <memory>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "haunted/core/Defs.h"
#include "haunted/core/Key.h"
#include "haunted/core/Terminal.h"
#include "haunted/ui/Label.h"
#include "haunted/ui/Textbox.h"
#include "haunted/ui/TextInput.h"
#include "haunted/ui/boxes/ExpandoBox.h"
#include "haunted/ui/boxes/PropoBox.h"
#include "haunted/ui/boxes/SwapBox.h"

#include "pingpong/core/Defs.h"

#include "spjalla/lines/Error.h"
#include "spjalla/lines/Success.h"
#include "spjalla/lines/Warning.h"

#include "spjalla/ui/Renderer.h"
#include "spjalla/ui/Window.h"

namespace Spjalla {
	class Client;
}

namespace Spjalla::UI {
	class Interface {
		friend class Spjalla::Client;

		private:
			Haunted::Terminal *terminal;
			Client *parent;

			std::list<Window *> windows;
			Window *statusWindow, *activeWindow;
			
			Window *overlay;

			/** Whenever the overlay is activated, a pointer to the previous window is kept in this variable so that the
			 *  correct window can be restored when the overlay is closed. */
			Window *beforeOverlay = nullptr;

		public:
			Haunted::UI::Boxes::SwapBox    *swappo;
			Haunted::UI::Boxes::ExpandoBox *expando;
			Haunted::UI::Label     *titlebar, *statusbar;
			Haunted::UI::TextInput *input;

		private:
			size_t winCount = 0;

			/** Sets up the labels, overlay and textinput. */
			void initBasic();

			/** Sets up the swapbox that contains all the windows. */
			void initSwappo();

			/** Sets up the expandobox that serves as the program's root control. */
			void initExpando();

			/** Closes a window. */
			void removeWindow(Window *);

			/** Renders a channel's user list onto the overlay. */
			void updateOverlay(const std::shared_ptr<PingPong::Channel> &);

			/** Renders a user's channel list onto the overlay. */
			void updateOverlay(const std::shared_ptr<PingPong::User> &);

			/** Renders a channel's topic onto the titlebar. */
			void updateTitlebar(const std::shared_ptr<PingPong::Channel> &);

			/** Returns the iterator pointing to the active window in swappo's children. */
			Haunted::UI::Container::Type::iterator windowIterator() const;

			/** Returns whether it's okay to immediately remove a given window. */
			bool canRemove(Window *) const;

			/** Sets the terminal's on_interrupt function. */
			void setInterrupt();

		public:
			std::function<void(Window *)> updateStatusbarFunction;
			unsigned int scrollBuffer = 0;
			Renderer renderer;

			Interface(Haunted::Terminal &, Client &);

			/** Redraws the interface. */
			void draw();

			/** Starts any workers (or the like) needed for this interface. */
			void init();

			void postinit();

			/** Logs a line of output for a given target window. */
			template <typename T>
			void log(const T &line, Window *window = nullptr) {
				if (window == nullptr)
					window = statusWindow;
				*window += line;
			}

			/** Logs a line of output for a given target window. */
			void log(const std::string &, Window * = nullptr);

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			template <typename T>
			void log(const T &line, const std::string &window_name) {
				log(line, getWindow(window_name));
			}

			/** Logs a line of output for a given target name. This can be `status` for the main window,
			 *  `networkname/#channel` for a channel or `networkname/nickname` for a private conversation. */
			void log(const std::string &line, const std::string &window_name);

			/** Logs an exception to the status window. */
			void log(const std::exception &);

			void warn(const std::string &warning, PingPong::Server *server = nullptr) {
				Lines::WarningLine wline {parent, warning};
				wline.server = server;
				log(wline);
			}

			template <typename W, typename D>
			void warn(const W &warning, const D &destination, PingPong::Server *server = nullptr) {
				Lines::WarningLine wline {parent, warning};
				wline.server = server;
				log(wline, destination);
			}

			void error(const std::string &error, PingPong::Server *serv = nullptr) {
				Lines::ErrorLine eline {parent, error};
				eline.server = serv;
				log(eline);
			}

			template <typename E, typename D>
			void error(const E &error, const D &destination, PingPong::Server *server = nullptr) {
				Lines::ErrorLine eline {parent, error};
				eline.server = server;
				log(eline, destination);
			}

			void success(const std::string &message, PingPong::Server *server = nullptr) {
				Lines::SuccessLine eline {parent, message};
				eline.server = server;
				log(eline);
			}

			template <typename E, typename D>
			void success(const E &message, const D &destination, PingPong::Server *server = nullptr) {
				Lines::SuccessLine eline {parent, message};
				eline.server = server;
				log(eline, destination);
			}

			/** Focuses a window. Note that this method will swap the active window and the given window, so the pointer
			 *  given will point to a different window after the method is called (assuming the given window isn't
			 *  already the active window, in which case nothing would happen). */
			void focusWindow(Window * = nullptr);
			void focusWindow(const std::string &);

			/** Tries to focus a window at a given index. Returns true if a window was found at the index. */
			bool focusWindow(size_t);

			/** Moves the window to the specified (zero-based) index. Returns the new index (usually the same as the
			 *  argument) or -1 if the given window is null or not part of this interface. */
			ssize_t moveWindow(Window *, size_t);

			/** Returns a pointer to the status window. */
			Window * getWindow() const { return statusWindow; }

			/** Returns a pointer to the window indicated by a given string. If no window is found, one will be created
			 *  with that name if `create` is true. */
			Window * getWindow(const std::string &, bool create = false, WindowType = WindowType::Other);

			/** Returns a pointer to the window corresponding to a given channel. If no window is found, one will be
			 *  created for the channel if `create` is true. */
			Window * getWindow(const std::shared_ptr<PingPong::Channel> &, bool create = false);

			/** Returns a pointer to the window corresponding to a given user. If no window is found, one will be
			 *  created for the user if `create` is true. */
			Window * getWindow(const std::shared_ptr<PingPong::User> &, bool create = false);

			/** Creates a new window, configures it as appropriate and appends it to the swapbox. */
			Window * newWindow(const std::string &name, WindowType);

			template <typename T>
			T * newWindow(const std::string &name, bool force = false) {
				if (!force)
					for (Window *window: windows)
						if (window->getName() == name)
							return dynamic_cast<T *>(window);

				T *window = new T(swappo, swappo->getPosition(), name);
				// window->set_name("window" + std::to_string(++winCount));
				window->setTerminal(nullptr); // inactive windows are marked by their null terminals
				window->setAutoscroll(true);
				windows.push_back(window);
				window->scrollBuffer = scrollBuffer;
				return window;
			}

			/** Switches to the next server in the list. */
			void nextServer();

			/** Switches to the next window after the current window. */
			void nextWindow();

			/** Switches to the previous window before the current window. */
			void previousWindow();

			/** Toggles mouse tracking. */
			void toggleMouse();

			/** Updates the text in the status bar. */
			void updateStatusbar();

			/** Updates the text in the overlay. */
			void updateOverlay();

			/** Updates the text in the titlebar. */
			void updateTitlebar();

			/** Toggles the overlay and returns a pointer to the window that was covered/uncovered. */
			Window * toggleOverlay();

			/** Returns all windows (for channels or private conversations) where a given user is present. */
			std::deque<Window *> windowsForUser(std::shared_ptr<PingPong::User>) const;

			/** Returns the window for a given channel. */
			Window * windowForChannel(std::shared_ptr<PingPong::Channel>) const;

			/** Returns the active window. ¯\_(ツ)_/¯ */
			Window * getActiveWindow() { return activeWindow; }

			/** If the active window (or the window below the overlay, if the overlay is active) is for a channel,
			 *  this returns the pointer to the relevant channel. */
			std::shared_ptr<PingPong::Channel> getActiveChannel() const;

			/** If the active window is for a user, this returns the pointer to the relevant user. */
			std::shared_ptr<PingPong::User> getActiveUser() const;

			/** Returns true if the window is active or currently covered by the overlay. */
			bool isActive(const Window *) const;

			/** Returns whether the overlay is currently visible. */
			bool overlayVisible() const;

			/** Scrolls the active window (unless it's the overlay) by half a page. */
			void scrollPage(bool up);

			/** Handles keypresses that aren't handled by the textinput. */
			bool onKey(const Haunted::Key &);

			/** Handles tab completion. */
			void tabComplete();

			/** Sets the content of the textinput. */
			void setInput(const std::string &);

			/** Gets the content of the textinput. */
			std::string getInput() const;

			/** Sets the scroll buffer value and updates all windows accordingly. */
			void setScrollBuffer(unsigned int);

			bool isOverlay(UI::Window *window) const { return window == overlay; }

			/** Sets the foreground color of the titlebar and statusbar. */
			void setBarForeground(ansi::color);
			/** Sets the background color of the titlebar and statusbar. */
			void setBarBackground(ansi::color);
			/** Sets the foreground color of the overlay. */
			void setOverlayForeground(ansi::color);
			/** Sets the background color of the overlay. */
			void setOverlayBackground(ansi::color);
			/** Sets the foreground color of the textinput. */
			void setInputForeground(ansi::color);
			/** Sets the background color of the textinput. */
			void setInputBackground(ansi::color);
	};
}

#endif
