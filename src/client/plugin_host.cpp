#include <filesystem>

#include <dlfcn.h>

#include "spjalla/core/options.h"
#include "spjalla/core/plugin_host.h"

namespace spjalla::plugins {
	plugin_host::~plugin_host() {
		inputhandlers_post.clear();
	}

	void plugin_host::close_plugins() {
		DBG("Closing plugins.");

		std::vector<void *> handles;
		handles.reserve(plugins.size());

		for (const plugin_tuple &tuple: plugins) {
			DBG("Closing " << std::get<0>(tuple));
			std::get<1>(tuple)->cleanup(this);
			handles.push_back(std::get<2>(tuple));
		}

		plugins.clear();

		for (void *handle: handles)
			dlclose(handle);

		DBG("Done closing plugins.");
	}

	plugin_host::plugin_tuple plugin_host::load_plugin(const std::string &path) {
		void *lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (lib == nullptr)
			throw std::runtime_error("dlopen returned nullptr");
		
		plugins::plugin *plugin = static_cast<plugins::plugin *>(dlsym(lib, PLUGIN_GLOBAL_VARIABLE_NAME));
		if (plugin == nullptr)
			throw std::runtime_error("Plugin is null");
		
		plugins.push_back({path, plugin, lib});
		return {path, plugin, lib};
	}

	void plugin_host::load_plugins(const std::string &path) {
		for (const auto &entry: std::filesystem::directory_iterator(path)) {
			if (entry.is_directory()) {
				load_plugins(entry.path());
			} else {
				const std::string extension = entry.path().extension();
				if (extension == ".so" || extension == ".dylib")
					load_plugin(entry.path().c_str());
			}
		}
	}

	void plugin_host::preinit_plugins() {
		for (const plugin_tuple &tuple: plugins)
			std::get<1>(tuple)->preinit(this);
	}

	void plugin_host::postinit_plugins() {
		for (const plugin_tuple &tuple: plugins)
			std::get<1>(tuple)->postinit(this);
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
		auto iter = std::find_if(plugins.begin(), plugins.end(), [&](const plugin_tuple &tuple) {
			return std::get<0>(tuple) == path;
		});

		return iter == plugins.end()? nullptr : std::get<1>(*iter);
	}

	// void plugin_host::cleanup() {
	// 	plugin_command_handlers.clear();
	// 	keyhandlers_pre.clear();
	// 	keyhandlers_post.clear();
	// 	inputhandlers_pre.clear();
	// 	inputhandlers_post.clear();
	// }
}
