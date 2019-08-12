#ifndef CORE_SPJALLA_H_
#define CORE_SPJALLA_H_

#include <functional>
#include <map>
#include <thread>
#include <tuple>

#include "core/defs.h"
#include "core/input_line.h"
#include "core/irc.h"
#include "messages/message.h"
#include "ui/interface.h"

namespace spjalla {
	class client {
		using command_handler = std::function<void(pingpong::server_ptr, const input_line &)>;
		// Tuple: (minimum args, maximum args, needs server, function)
		using command_tuple = std::tuple<int, int, bool, command_handler>;
		using command_pair = std::pair<std::string, command_tuple>;

		private:
			pingpong::irc pp;
			std::multimap<std::string, command_tuple> command_handlers;
			std::shared_ptr<std::thread> input_thread;
			std::mutex pp_mux;
			bool alive = true;
			haunted::terminal term;
			interface ui;

		public:
			client(): term(haunted::terminal(std::cin, ansi::ansistream())), ui(&term) {}

			client(client &&) = delete;
			client(const client &) = delete;

			~client();

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			client & operator+=(const command_pair &p);

			/**
			 * Adds a server.
			 * @param ptr A pointer to a server.
			 */
			client & operator+=(const pingpong::server_ptr &ptr);
			
			/**
			 * Succinctly adds a handler for a single-argument command.
			 * @param cmd The name of the command as typed by the user.
			 */
			template <typename T>
			void add(const std::string &cmd) { add<T>(cmd, true); }
			template <typename T>
			void add(const std::string &cmd, bool needs_serv) {
				*this += {cmd, {1, 1, needs_serv, [&](pingpong::server_ptr serv, const input_line &il) {
					T(serv, il.args[0]).send(true);
				}}};
			}

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			void add(const command_pair &p);

			/**
			 * Initializes the client.
			 */
			void init();

			/**
			 * Creates a thread to receive input.
			 * @see input_worker
			 */
			void start_input();

			void join();

			void stop();

			/**
			 * Reads input from the server socket in an infinite loop and handles data as it comes in.
			 * @param ptr A pointer to a server.
			 */
			void input_worker();

			/**
			 * Processes a line of user input.
			 * @param  line A line of user input.
			 * @return Whether the line was recognized as a valid input.
			 */
			bool handle_line(const input_line &line);

			/**
			 * Adds listeners for pingpong events.
			 */
			void add_listeners();

			/**
			 * Adds the built-in command handlers.
			 */
			void add_handlers();

			pingpong::server_ptr active_server();
			pingpong::channel_ptr active_channel();
			std::string active_nick();
	};
}

#endif
