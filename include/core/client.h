#ifndef SPJALLA_CORE_CLIENT_H_
#define SPJALLA_CORE_CLIENT_H_

#include <functional>
#include <list>
#include <map>
#include <thread>
#include <tuple>

#include "haunted/core/hdefs.h"
#include "pingpong/messages/message.h"
#include "pingpong/core/irc.h"

#include "core/input_line.h"
#include "core/plugin_host.h"
#include "core/tab_completion.h"
#include "config/config.h"
#include "plugins/plugin.h"
#include "ui/interface.h"
#include "ui/status_widget.h"

namespace spjalla {
	class client: public plugins::plugin_host {
		friend class ui::interface;

		using command_handler = std::function<void(pingpong::server *, const input_line &)>;
		// Tuple: (minimum args, maximum args, needs server, function)
		using command_tuple = std::tuple<int, int, bool, command_handler, completer>;
		using command_pair = std::pair<std::string, command_tuple>;

// client/client.cpp

		private:
			pingpong::irc irc;
			std::multimap<std::string, command_tuple> command_handlers;
			std::mutex irc_mutex;
			ansi::ansistream &out_stream;
			haunted::terminal term;
			ui::interface ui;
			config::database configs;

			template <typename T>
			ui::window * try_window(const T &where) {
				ui::window *win = ui.get_window(where, false);
				if (!win)
					ui.log(ansi::yellow("!!") + " Couldn't find window for " + std::string(*where));
				return win;
			}

			/** Logs a message indicated that there is no active channel. */
			void no_channel();

			/** Handles commands like /kick that take a user and an optional longer string and an optional channel.
			 *  If no channel is specified, the command must be issued from a channel window.
			 *  Returns whether a "no active channel" message should be displayed. */
			template <typename T>
			bool triple_command(pingpong::server *serv, const input_line &il, std::shared_ptr<pingpong::channel> chan) {
				const std::string first = il.first(), rest = il.rest();
				const size_t spaces = std::count(il.body.begin(), il.body.end(), ' ');

				if (first.front() == '#') {
					const std::string &where = first;
					if (spaces == 1) {
						T(serv, where, rest).send();
					} else {
						const size_t found = rest.find(' ');
						T(serv, where, rest.substr(0, found), rest.substr(found + 1)).send();
					}
				} else if (chan) {
					T(serv, chan, first, rest).send();
				} else {
					return true;
				}

				return false;
			}

		public:
			client(int heartbeat_period_ = 100);

			client(const client &) = delete;
			client(client &&) = delete;
			client & operator=(const client &) = delete;
			client & operator=(client &&) = delete;
			~client();

			/** Returns the ID of the active server. */
			std::string active_server_id();

			/** Returns the hostname of the active server. */
			std::string active_server_hostname();

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			client & operator+=(const command_pair &p);

			/** Adds a server. */
			client & operator+=(pingpong::server *ptr);
			
			/**
			 * Succinctly adds a handler for a single-argument command.
			 * @param cmd The name of the command as typed by the user.
			 */
			template <typename T>
			void add(const std::string &cmd, bool needs_serv = true) {
				*this += {cmd, {1, 1, needs_serv, [&](pingpong::server *serv, const input_line &il) {
					T(serv, il.args[0]).send();
				}, {}}};
			}

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			void add(const command_pair &p);

			/** Initializes the client. */
			void init();

			/** Processes a line of user input and returns whether the line was recognized as a valid input. */
			bool handle_line(const input_line &line);

			/** Updates the interface to accommodate the removal of a server. */
			void server_removed(pingpong::server *);

			/** Joins any threads associated with the client. */
			void join();

			/** Returns the client's ui::interface instance. */
			ui::interface & get_ui() { return ui; }

			/** Returns a pointer to the active server. */
			pingpong::server * active_server();

			/** Returns the nickname in use on the active server if possible, or a blank string otherwise. */
			std::string active_nick();

			template <typename T>
			void log(const T &obj) {
				ui.log(obj);
			}

			template <typename T, typename P>
			void log(const T &obj, P *ptr) {
				ui.log(obj, ptr);
			}

// client/commands.cpp

		private:
			/** Handles the parsing for the /ban command. */
			void ban(pingpong::server *, const input_line &, const std::string &type = "+b");

			/** Prints debug information about the server list to the log file. */
			void debug_servers();

		public:
			/** Adds the built-in command handlers. */
			void add_commands();

// client/events.cpp

			/** Adds listeners for pingpong events. */
			void add_events();

// client/heartbeat.cpp

		private:
			bool heartbeat_alive = false;

			/** A thread that executes actions at regular intervals. */
			std::thread heartbeat;

			/** The number of milliseconds to wait between heartbeats. */
			int heartbeat_period;

			/** Contains all the functions to execute on each heartbeat. */
			std::list<std::function<void(int)>> heartbeat_listeners {};

			/** Keeps executing all the heartbeat listeners and waiting for the heartbeat period. Stops when
			 *  heartbeat_alive turns false. */
			void heartbeat_loop();

		public:
			/** Adds a function to the list of heartbeat listeners. The heartbeat period is passed as an argument. */
			void add_heartbeat_listener(const std::function<void(int)> &);

			/** Starts the heartbeat thread, which executes all the heartbeat listeners at regular intervals. */
			void init_heartbeat();

// client/input.cpp

			/** Adds a listener to the textinput that processes its contents. */
			void add_input_listener();

			/** Tries to expand a command (e.g., "mod" → "mode"). Returns a vector of all matches. */
			std::vector<std::string> command_matches(const std::string &);

			input_line get_input_line(const std::string &) const;

			void tab_complete();

// client/statusbar.cpp

		private:
			std::deque<std::shared_ptr<ui::status_widget>> status_widgets;

			/** Sorts all the statusbar widgets by priority. */
			void sort_widgets();

		public:
			void add_status_widget(std::shared_ptr<ui::status_widget> widget);

			void init_statusbar();

			void render_statusbar();
	};
}

#endif
