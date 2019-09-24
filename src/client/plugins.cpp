#include <filesystem>

#include <dlfcn.h>

#include "core/client.h"
#include "core/spopt.h"
#include "plugins/plugin.h"

namespace spjalla {
	bool client::before_send(pingpong::command &command) {
		bool should_send = true;
		[&] {
			for (const plugins::priority priority: {plugins::priority::high, plugins::priority::normal,
			     plugins::priority::low}) {
				for (auto &function: plugin_command_handlers.at(priority)) {
					plugins::command_result result = function(&command, should_send);

					if (result == plugins::command_result::kill || result == plugins::command_result::disable) {
						should_send = false;
					} else if (result == plugins::command_result::approve
					        || result == plugins::command_result::enable) {
						should_send = true;
					}

					if (result == plugins::command_result::kill || result == plugins::command_result::approve)
						return;
				}
			}
		}();

		return should_send;
	}
	
	plugins::plugin * client::load_plugin(const std::string &path) {
		void *lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (lib == nullptr)
			throw std::runtime_error("dlopen returned nullptr");
		
		plugins::plugin *plugin = static_cast<plugins::plugin *>(dlsym(lib, PLUGIN_GLOBAL_VARIABLE_NAME));
		if (plugin == nullptr)
			throw std::runtime_error("Plugin is null");
		
		plugins.push_back(plugin);
		return plugin;
	}

	void client::load_plugins(const std::string &path) {
		for (const auto &entry: std::filesystem::directory_iterator(path))
			load_plugin(entry.path().c_str());
	}
}
