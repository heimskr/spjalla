#ifndef SPJALLA_CORE_PLUGIN_HOST_H_
#define SPJALLA_CORE_PLUGIN_HOST_H_

#include <map>
#include <string>
#include <vector>

#include "haunted/core/key.h"

#include "pingpong/commands/command.h"

#include "spjalla/core/input_line.h"
#include "spjalla/plugins/plugin.h"

namespace spjalla::plugins {
	class plugin_host {
		using plugin_pair = std::pair<std::string, plugins::plugin *>; // path, plugin

		template <typename T>
		// The bool argument indicates whether the result hasn't been disabled.
		using pre_function = std::function<plugins::cancelable_result(T &, bool)>;

		template <typename T>
		using post_function = std::function<void(const T &)>;

		private:
			std::vector<plugin_pair> plugins {};
			std::map<plugins::priority, std::vector<pre_function<pingpong::command>>> plugin_command_handlers =
				{{plugins::priority::high, {}}, {plugins::priority::normal, {}}, {plugins::priority::low, {}}};

			/** Holds prehandlers for keypresses. Note that keypresses handled by the textinput aren't passed on to pre-
			 *  or posthandlers. */
			std::vector<pre_function<haunted::key>>  keyhandlers_pre  {};

			/** Holds posthandlers for keypresses. Note that keypresses handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::vector<post_function<haunted::key>> keyhandlers_post {};

			/** Holds prehandlers for input lines. Note that input lines handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::vector<pre_function<input_line>>  inputhandlers_pre  {};

			/** Holds posthandlers for input lines. Note that input lines handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::vector<post_function<input_line>> inputhandlers_post {};

			/** Determines whether a pre-event should go through. */
			template <typename T>
			bool before(T &obj, const std::vector<pre_function<T>> &funcs) {
				return before_multi(obj, funcs).first;
			}

			/** Determines whether a pre-event should go through. Used inside functions like plugin_host::before_send
			 *  that process sets of sets of handler functions. */
			template <typename T>
			std::pair<bool, handler_result> before_multi(T &obj, const std::vector<pre_function<T>> &funcs,
			bool initial = true) {
				bool should_send = initial;
				for (auto &func: funcs) {
					plugins::cancelable_result result = func(obj, should_send);

					if (result == plugins::cancelable_result::kill || result == plugins::cancelable_result::disable) {
						should_send = false;
					} else if (result == plugins::cancelable_result::approve
					        || result == plugins::cancelable_result::enable) {
						should_send = true;
					}

					if (result == plugins::cancelable_result::kill || result == plugins::cancelable_result::approve)
						return {should_send, handler_result::kill};
				}

				return {should_send, handler_result::pass};
			}

			template <typename T>
			void after(const T &obj, const std::vector<post_function<T>> &funcs) {
				for (auto &func: funcs)
					func(obj);
			}

		public:
			virtual ~plugin_host() = 0;

			/** Loads a plugin from a given shared object. */
			plugin_pair load_plugin(const std::string &path);

			/** Loads all plugins in a given directory. */
			void load_plugins(const std::string &path);

			/** Initializes all loaded plugins before client initialization. */
			void preinit_plugins();

			/** Initializes all loaded plugins after client initialization. */
			void postinit_plugins();

			/** Determines whether a command can be sent. Returns true if the command should be sent, or false if a
			 *  plugin chose to block it. */
			bool before_send(pingpong::command &);

			/** Determines whether a key should be processed by the client. Returns true if so, or false if a plugin
			 *  chose to block the key. Can modify the input. */
			bool before_key(haunted::key &key) {
				return before(key, keyhandlers_pre);
			}

			bool before_input(input_line &il) {
				return before(il, inputhandlers_pre);
			}

			void after_input(const input_line &il) {
				return after(il, inputhandlers_post);
			}

			/** If a plugin was loaded from a given path, a pointer to its corresponding plugin object is returned. */
			plugins::plugin * plugin_for_path(const std::string &path) const;

			/** Registers a handler for commands. */
			void handle(const pre_function<pingpong::command> &func, plugins::priority priority) {
				plugin_command_handlers[priority].push_back(func);
			}

			/** Registers a handler to handle keypresses before the client handles them and determine whether the client
			 *  will handle them. */
			void handle_pre(const pre_function<haunted::key> &func) {
				keyhandlers_pre.push_back(func);
			}

			/** Registers a handler to handle keypresses after the client has handled them. */
			void handle_post(const post_function<haunted::key> &func) {
				// TODO: implement posthandlers for keypresses.
				keyhandlers_post.push_back(func);
			}

			/** Registers a handler to handle input lines before the client handles them and determine whether the
			 *  client will handle them. */
			void handle_pre(const pre_function<input_line> &func) {
				inputhandlers_pre.push_back(func);
			}

			/** Registers a handler to handle keypresses after the client has handled them. */
			void handle_post(const post_function<input_line> &func) {
				inputhandlers_post.push_back(func);
			}
	};
}

#endif
