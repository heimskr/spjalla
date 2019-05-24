#ifndef CORE_SPJALLA_H_
#define CORE_SPJALLA_H_

#include <functional>
#include <map>
#include <tuple>

#include "core/defs.h"
#include "core/input_line.h"
#include "core/server.h"
#include "messages/message.h"

namespace spjalla {
	class client {
		using command_handler = std::function<void(pingpong::server_ptr, const input_line &)>;
		using command_tuple = std::tuple<int, int, command_handler>;
		using command_pair = std::pair<std::string, command_tuple>;

		private:
			std::shared_ptr<pingpong::irc> pp;
			std::multimap<std::string, command_tuple> command_handlers;

		public: 
			client(std::shared_ptr<pingpong::irc> irc_): pp(irc_) {}

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			client & operator+=(const command_pair &p);
			
			/**
			 * Succinctly adds a handler for a single-argument command.
			 * @param cmd The name of the command as typed by the user.
			 */
			template <typename T>
			void add(const std::string &cmd) {
				*this += {cmd, {1, 1, [&](pingpong::server_ptr serv, const input_line &il) {
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
			 * Reads input from the server socket in an infinite loop and handles data as it comes in.
			 * @param ptr A pointer to a server.
			 */
			void input_worker(pingpong::server_ptr ptr);

			/**
			 * Processes a line of user input.
			 * @param  ptr  A pointer to a server.
			 * @param  line A line of user input.
			 * @return Whether the line was recognized as a valid input.
			 */
			bool handle_line(pingpong::server_ptr ptr, const input_line &line);

			/**
			 * Adds listeners for pingpong events.
			 */
			void add_listeners();

			/**
			 * Adds the built-in command handlers.
			 */
			void add_handlers();
	};
}

#endif
