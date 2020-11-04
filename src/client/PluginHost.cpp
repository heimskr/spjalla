#include <filesystem>

#include <dlfcn.h>

#include "spjalla/core/Options.h"
#include "spjalla/core/PluginHost.h"

namespace Spjalla::Plugins {
	PluginHost::~PluginHost() {}

	void PluginHost::unloadPlugin(PluginTuple &tuple) {
		std::get<1>(tuple)->cleanup(this);
		dlclose(std::get<2>(tuple));
		auto iter = std::find(plugins.begin(), plugins.end(), tuple);
		if (iter == plugins.end())
			throw std::runtime_error("Couldn't find plugin tuple: " + std::get<0>(tuple));
		plugins.erase(iter);
	}

	void PluginHost::unloadPlugins() {
		while (!plugins.empty())
			unloadPlugin(plugins.front());
		plugins.clear();
	}

	PluginHost::PluginTuple PluginHost::loadPlugin(const std::string &path) {
		if (!std::filesystem::exists(path)) {
			throw std::filesystem::filesystem_error("No plugin found at path",
				std::error_code(ENOENT, std::generic_category()));
		}

		void *lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (lib == nullptr)
			throw std::runtime_error("dlopen returned nullptr");
		
		Plugins::Plugin *plugin = static_cast<Plugins::Plugin *>(dlsym(lib, PLUGIN_GLOBAL_VARIABLE_NAME));
		if (plugin == nullptr)
			throw std::runtime_error("Plugin is null");
		
		plugins.push_back({path, plugin, lib});
		return {path, plugin, lib};
	}

	void PluginHost::loadPlugins(const std::string &path) {
		for (const auto &entry: std::filesystem::directory_iterator(path)) {
			if (entry.is_directory()) {
				loadPlugins(entry.path());
			} else {
				const std::string extension = entry.path().extension();
				if (extension == ".so" || extension == ".dylib")
					loadPlugin(entry.path().c_str());
			}
		}
	}

	PluginHost::PluginTuple * PluginHost::getPlugin(const std::string &name, bool insensitive) {
		decltype(plugins)::iterator iter;
		if (insensitive) {
			const std::string lower = formicine::util::lower(name);
			iter = std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
				return std::get<0>(tuple) == name || formicine::util::lower(std::get<1>(tuple)->getName()) == lower;
			});
		} else {
			iter = std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
				return std::get<0>(tuple) == name || std::get<1>(tuple)->getName() == name;
			});
		}

		return iter == plugins.end()? nullptr : &*iter;
	}

	PluginHost::PluginTuple * PluginHost::getPlugin(const Plugins::Plugin *plugin) {
		auto iter = std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
			return std::get<1>(tuple) == plugin;
		});

		return iter == plugins.end()? nullptr : &*iter;
	}

	bool PluginHost::hasPlugin(const std::filesystem::path &path) const {
		return plugins.end() != std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
			return path == std::get<0>(tuple);
		});
	}

	bool PluginHost::hasPlugin(const std::string &name, bool insensitive) const {
		if (insensitive) {
			const std::string lower = formicine::util::lower(name);
			return plugins.end() != std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
				return formicine::util::lower(std::get<1>(tuple)->getName()) == lower;
			});
		}

		return plugins.end() != std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
			return std::get<1>(tuple)->getName() == name;
		});
	}

	const std::list<PluginHost::PluginTuple> & PluginHost::getPlugins() const {
		return plugins;
	}

	void PluginHost::preinitPlugins() {
		for (const PluginTuple &tuple: plugins)
			std::get<1>(tuple)->preinit(this);
	}

	void PluginHost::postinitPlugins() {
		for (const PluginTuple &tuple: plugins)
			std::get<1>(tuple)->postinit(this);
	}

	bool PluginHost::beforeSend(PingPong::Command &command) {
		bool should_send = true;
		for (auto priority: {Plugins::Priority::High, Plugins::Priority::Normal, Plugins::Priority::Low}) {
			HandlerResult result = HandlerResult::Pass;
			std::tie(should_send, result) = beforeMulti(command, pluginCommandHandlers.at(priority), should_send);
			if (result == HandlerResult::Kill)
				break;
		}

		return should_send;
	}

	Plugins::Plugin * PluginHost::pluginForPath(const std::string &path) const {
		auto iter = std::find_if(plugins.begin(), plugins.end(), [&](const PluginTuple &tuple) {
			return std::get<0>(tuple) == path;
		});

		return iter == plugins.end()? nullptr : std::get<1>(*iter);
	}
}
