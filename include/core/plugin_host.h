#ifndef SPJALLA_CORE_PLUGIN_HOST_H_
#define SPJALLA_CORE_PLUGIN_HOST_H_

#include <map>
#include <string>
#include <vector>

#include "plugins/plugin.h"

namespace spjalla::plugins {
	class plugin_host {
		using plugin_pair = std::pair<std::string, plugins::plugin *>; // path, plugin

		private:
			std::vector<plugin_pair> plugins {};
			std::map<plugins::priority, std::vector<std::function<plugins::command_result(pingpong::command *, bool)>>>
				plugin_command_handlers = // The bool argument indicates whether the result hasn't been disabled.
				{{plugins::priority::high, {}}, {plugins::priority::normal, {}}, {plugins::priority::low, {}}};

		public:
			/** Loads a plugin from a given shared object. */
			plugin_pair load_plugin(const std::string &path);

			/** Loads all plugins in a given directory. */
			void load_plugins(const std::string &path);

			/** Determines whether a command can be sent. Returns true if the command should be sent, or false if a
			 *  plugin chose to block it. */
			bool before_send(pingpong::command &);

			/** If a plugin was loaded from a given path, a pointer to its corresponding plugin object is returned. */
			plugins::plugin * plugin_for_path(const std::string &path) const;

			template <typename T, typename = std::enable_if_t<std::is_base_of<pingpong::command, T>::value>>
			void handle(const std::function<plugins::command_result(const T &, bool)> &fn, plugins::priority priority) {
				plugin_command_handlers[priority].push_back(fn);
			}
	};
}

#endif
