#ifndef SPJALLA_CORE_CLIENT_H_
#define SPJALLA_CORE_CLIENT_H_

#include <functional>
#include <list>
#include <map>
#include <thread>
#include <tuple>
#include <unordered_map>

#include "haunted/core/defs.h"
#include "haunted/core/key.h"

#include "pingpong/messages/message.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"


#include "spjalla/commands/command.h"

#include "spjalla/core/aliases.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/plugin_host.h"
#include "spjalla/core/tab_completion.h"

#include "spjalla/config/cache.h"
#include "spjalla/config/config.h"

#include "spjalla/plugins/plugin.h"

#include "spjalla/ui/interface.h"
#include "spjalla/ui/status_widget.h"

namespace spjalla {
	class client: public plugins::plugin_host {
		friend class ui::interface;

// client/client.cpp

		private:
			pingpong::irc irc;
			std::multimap<std::string, commands::command> command_handlers;
			std::mutex irc_mutex;
			ansi::ansistream &out_stream;
			haunted::terminal term;
			ui::interface ui;
			completions::command_completer completer;

#include "client.tcc"

		public:
			config::database configs;
			aliases alias_db;
			config::cache cache;

			client(int heartbeat_period_ = pingpong::util::precision / 10);

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
			client & operator+=(const spjalla::commands::pair &p);

			/** Adds a server. */
			client & operator+=(pingpong::server *ptr);

			/** Adds a command handler, given a pair that signifies the name of the command as typed by the user plus a
			 *  handler tuple. */
			void add(const spjalla::commands::pair &);
			void add(const std::string &, const spjalla::commands::command &);
			void add(const std::string &, int, int, bool, const commands::command::handler_fn &,
			         const completions::completion_fn & = {},
			         const std::vector<completions::completion_state::suggestor_fn> & = {});

			bool remove_command(const std::string &);

			/** Initializes the client. */
			void init();

			/** Performs actions after every plugin's postinit method has been called. */
			void postinit();

			/** Processes a line of user input and returns whether the line was recognized as a valid input. */
			bool handle_line(const input_line &);

			/** Updates the interface to accommodate the removal of a server. */
			void server_removed(pingpong::server *);

			/** Joins any threads associated with the client. */
			void join();

			/** Joins threads and unloads plugins. */
			// virtual void cleanup() override;

			/** Returns the client's ui::interface instance. */
			ui::interface & get_ui() { return ui; }

			/** Returns a reference to the IRC object.. */
			pingpong::irc & get_irc() { return irc; }

			/** Returns a reference to the terminal. */
			haunted::terminal & get_terminal() { return term; }

			/** Returns a pointer to the active server. */
			pingpong::server * active_server();

			/** Returns the nickname in use on the active server if possible, or a blank string otherwise. */
			std::string active_nick();

			/** Returns all the windows as haunted::ui::control pointers. */
			std::deque<haunted::ui::control *> get_window_controls() const;

			/** Opens a message window with a given nick. */
			ui::window * query(const std::string &, pingpong::server *);
			ui::window * query(std::shared_ptr<pingpong::user>);

			/** Logs a message indicated that there is no active channel. */
			void no_channel();

// stt/commands.cpp

		private:
			/** Handles the parsing for the /ban command. */
			void ban(pingpong::server *, const input_line &, const std::string &type = "+b");

		public:
			/** Adds the built-in command handlers. */
			void add_commands();

			/** Prints debug information about the server list to the log file. */
			void debug_servers();

// client/events.cpp

		private:
			using queue_fn = std::function<void()>;
			using queue_pair = std::pair<pingpong::server::stage, queue_fn>;
			std::unordered_map<pingpong::server *, std::list<queue_pair>> server_status_queue {};

			/** Calls and removes all functions in the server status queue waiting for a given server and status. */
			void call_in_queue(pingpong::server *, pingpong::server::stage);

		public:
			/** Adds listeners for pingpong events. */
			void add_events();

			/** Adds a function to a queue to be called when a server reaches a given stage. */
			void wait_for_server(pingpong::server *, pingpong::server::stage, const queue_fn &);

// client/heartbeat.cpp

		public:
			using heartbeat_listener = std::shared_ptr<std::function<void(int)>>;

		private:
			bool heartbeat_alive = false;

			/** A thread that executes actions at regular intervals. */
			std::thread heartbeat;

			/** The duration to wait between heartbeats. */
			int heartbeat_period;

			/** Contains all the functions to execute on each heartbeat. */
			std::list<heartbeat_listener> heartbeat_listeners {};

			/** Keeps executing all the heartbeat listeners and waiting for the heartbeat period. Stops when
			 *  heartbeat_alive turns false. */
			void heartbeat_loop();

		public:
			/** Adds a function to the list of heartbeat listeners. The heartbeat period is passed as an argument. */
			void add_heartbeat_listener(const heartbeat_listener &);

			/** Removes a function from the list of heartbeat listeners. */
			void remove_heartbeat_listener(const heartbeat_listener &);

			/** Starts the heartbeat thread, which executes all the heartbeat listeners at regular intervals. */
			void init_heartbeat();

// client/input.cpp

			/** Adds a listener to the textinput that processes its contents. */
			void add_input_listener();

			/** Tries to expand a command (e.g., "mod" → "mode"). Returns a vector of all matches. */
			std::vector<std::string> command_matches(const std::string &);

			input_line get_input_line(const std::string &) const;

// client/statusbar.cpp

		private:
			std::list<std::shared_ptr<ui::status_widget>> status_widgets;

			/** Sorts all the statusbar widgets by priority. */
			void sort_widgets();

		public:
			void add_status_widget(std::shared_ptr<ui::status_widget>);

			bool remove_status_widget(std::shared_ptr<ui::status_widget>);

			const std::list<std::shared_ptr<ui::status_widget>> & get_status_widgets() const { return status_widgets; }

			void init_statusbar();

			void render_statusbar();

// client/tab_completion.cpp

			std::unordered_map<std::string, completions::completion_state> completion_states;

			void tab_complete();

			void complete_message(std::string &, size_t cursor, ssize_t word_offset = 0);

			void key_postlistener(const haunted::key &);
	};
}

#endif
