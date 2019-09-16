#ifndef SPJALLA_CORE_CLIENT_H_
#define SPJALLA_CORE_CLIENT_H_

#include <functional>
#include <map>
#include <thread>
#include <tuple>

#include "core/hdefs.h"
#include "core/input_line.h"
#include "core/irc.h"
#include "messages/message.h"
#include "ui/interface.h"

namespace spjalla {
	class client {
		friend class ui::interface;

		using command_handler = std::function<void(pingpong::server_ptr, const input_line &)>;
		// Tuple: (minimum args, maximum args, needs server, function)
		using command_tuple = std::tuple<int, int, bool, command_handler>;
		using command_pair = std::pair<std::string, command_tuple>;

		private:
			pingpong::irc pp;
			std::multimap<std::string, command_tuple> command_handlers;
			std::mutex pp_mux;
			bool alive = true;
			ansi::ansistream &out_stream;
			haunted::terminal term;
			ui::interface ui;

			void debug_servers();

		public:
			client(): out_stream(ansi::out), term(haunted::terminal(std::cin, out_stream)), ui(&term, this) {}

			client(const client &) = delete;
			client(client &&) = delete;
			client & operator=(const client &) = delete;
			client & operator=(client &&) = delete;
			~client();

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			client & operator+=(const command_pair &p);

			/** Adds a server. */
			client & operator+=(const pingpong::server_ptr &ptr);
			
			/**
			 * Succinctly adds a handler for a single-argument command.
			 * @param cmd The name of the command as typed by the user.
			 */
			template <typename T>
			void add(const std::string &cmd, bool needs_serv = true) {
				*this += {cmd, {1, 1, needs_serv, [&](pingpong::server_ptr serv, const input_line &il) {
					T(serv, il.args[0]).send();
				}}};
			}

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			void add(const command_pair &p);

			/** Initializes the client. */
			void init();

			/** Stops the client. */
			void stop();

			/** Processes a line of user input and returns whether the line was recognized as a valid input. */
			bool handle_line(const input_line &line);

			/** Adds listeners for pingpong events. */
			void add_listeners();

			/** Adds the built-in command handlers. */
			void add_handlers();

			/** Updates the interface to accommodate the removal of a server. */
			void server_removed(pingpong::server_ptr);

			void join();

			ui::interface & get_ui() { return ui; }

			pingpong::server_ptr active_server();
			pingpong::channel_ptr active_channel();
			std::string active_nick();
	};
}

#endif
