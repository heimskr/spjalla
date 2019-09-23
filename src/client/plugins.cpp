#include <filesystem>

#include <dlfcn.h>

#include "client/plugins.h"
#include "core/spopt.h"

namespace spjalla::mixins {
	plugins::plugin * client_plugins::load_plugin(const std::string &path) {
		void *lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (lib == nullptr)
			throw std::runtime_error("dlopen returned nullptr");
		
		plugins::plugin *plugin = static_cast<plugins::plugin *>(dlsym(lib, PLUGIN_GLOBAL_VARIABLE_NAME));
		if (plugin == nullptr)
			throw std::runtime_error("Plugin is null");
		
		plugins.push_back(plugin);
		return plugin;
	}

	void client_plugins::load_plugins(const std::string &path) {
		for (const auto &entry: std::filesystem::directory_iterator(path))
			load_plugin(entry.path().c_str());
	}
}
