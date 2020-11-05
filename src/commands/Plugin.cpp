#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"

namespace Spjalla::Commands {
	namespace Plugin {
		void info(Client &client, const int argc, const InputLine &il) {
			if (argc < 2) {
				client.warn("Usage: " + "/plugin info "_b + "<plugin name>"_bd);
				return;
			}

			const std::string name = formicine::util::trim(il.body.substr(il.body.find("info ") + 5));
			Plugins::PluginHost::PluginTuple *tuple = client.getPlugin(name, true);
			if (!tuple) {
				client.error("Couldn't find a plugin matching " + ansi::red(name) + ".");
			} else {
				Plugins::Plugin *plugin = std::get<1>(*tuple);
				client.log(ansi::bold(plugin->getName()) + "  v"_d + ansi::dim(plugin->getVersion()) + "  "
					+ plugin->getDescription());
			}
		}

		void list(Client &client, const int argc) {
			if (argc != 1) {
				client.warn("/plugin list"_b + " takes no arguments.");
				return;
			}

			const auto &plugins = client.getPlugins();
			if (plugins.empty()) {
				client.warn("No plugins are loaded.");
			} else {
				client.log("Plugins:");
				size_t max_name = 0, max_version = 0;

				for (const auto &tuple: plugins) {
					const size_t nlength = std::get<1>(tuple)->getName().length();
					const size_t vlength = std::get<1>(tuple)->getVersion().length();
					if (max_name < nlength)
						max_name = nlength;
					if (max_version < vlength)
						max_version = vlength;
				}

				for (const auto &tuple: plugins) {
					Plugins::Plugin *plugin = std::get<1>(tuple);
					const std::string name = plugin->getName(), version = plugin->getVersion(),
					                  desc = plugin->getDescription();
					client.log("    " + ansi::bold(name) + std::string(max_name - name.length(), ' ') + "  v"_d
						+ ansi::dim(version) + std::string(max_version - version.length() + 2, ' ') + desc);
				}
			}
		}

		void load(Client &client, const int argc, const InputLine &il) {
			if (argc < 2) {
				client.warn("Usage: " + "/plugin load "_b + "<plugin path>"_bd);
				return;
			}

			const std::string pathname = formicine::util::trim(il.body.substr(il.body.find("load ") + 5));
			if (client.hasPlugin(std::filesystem::path(pathname))) {
				client.error("The plugin at " + ansi::bold(pathname) + " is already loaded.");
				return;
			}

			try {
				Plugins::Plugin *plugin = std::get<1>(client.loadPlugin(pathname));
				plugin->preinit(&client);
				plugin->postinit(&client);
				client.success("Loaded " + ansi::bold(plugin->getName()) + ".");
			} catch (const std::filesystem::filesystem_error &err) {
				client.error(ansi::bold(pathname) + " doesn't exist.");
			}
		}

		void unload(Client &client, const int argc, const InputLine &il) {
			if (argc < 2) {
				client.warn("Usage: " + "/plugin unload "_b + "<plugin name>"_bd);
				return;
			}

			const std::string name = formicine::util::trim(il.body.substr(il.body.find("unload ") + 7));
			Plugins::PluginHost::PluginTuple *tuple = client.getPlugin(name, true);
			if (!tuple) {
				client.error("Couldn't find a plugin matching " + ansi::red(name) + ".");
			} else {
				const std::string plugin_name = std::get<1>(*tuple)->getName();
				client.unloadPlugin(*tuple);
				client.log("Unloaded " + ansi::bold(plugin_name) + ".");
			}
		}
	}

	void doPlugin(Client &client, const InputLine &il) {
		const size_t argc = il.args.size();
		if (argc == 0) {
			client.log("/plugin subcommands"_u);
			client.log("    info"_b + "  Shows information about a loaded plugin.");
			client.log("    list"_b + "  Lists all loaded plugins.");
			client.log("    load"_b + "  Loads a plugin by path.");
			client.log("  unload"_b + "  Unloads a plugin by path or name.");
			return;
		}

		const std::string sub = il.first();

		if (sub == "info") {
			Plugin::info(client, argc, il);
		} else if (sub == "list") {
			Plugin::list(client, argc);
		} else if (sub == "load") {
			Plugin::load(client, argc, il);
		} else if (sub == "unload") {
			Plugin::unload(client, argc, il);
		} else {
			client.error("Unknown /plugin subcommand: " + ansi::red(sub));
		}
	}
}
