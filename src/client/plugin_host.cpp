#include <dlfcn.h>

#include "core/spopt.h"
#include "core/plugin_host.h"

namespace spjalla::plugins {
	plugin_host::plugin_pair plugin_host::load_plugin(const std::string &path) {
		void *lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (lib == nullptr)
			throw std::runtime_error("dlopen returned nullptr");
		
		plugins::plugin *plugin = static_cast<plugins::plugin *>(dlsym(lib, PLUGIN_GLOBAL_VARIABLE_NAME));
		if (plugin == nullptr)
			throw std::runtime_error("Plugin is null");
		
		plugins.push_back({path, plugin});
		return {path, plugin};
	}

	void plugin_host::load_plugins(const std::string &path) {
		for (const auto &entry: std::filesystem::directory_iterator(path))
			load_plugin(entry.path().c_str());
	}

	bool plugin_host::before_send(pingpong::command &command) {
		bool should_send = true;
		for (auto priority: {plugins::priority::high, plugins::priority::normal, plugins::priority::low}) {
			handler_result result = handler_result::pass;
			std::tie(should_send, result) = before_multi(command, plugin_command_handlers.at(priority), should_send);
			if (result == handler_result::kill)
				break;
		}

		return should_send;
	}

	plugins::plugin * plugin_host::plugin_for_path(const std::string &path) const {
		auto iter = std::find_if(plugins.begin(), plugins.end(), [&](const plugin_pair &pair) {
			return pair.first == path;
		});

		return iter == plugins.end()? nullptr : iter->second;
	}
}