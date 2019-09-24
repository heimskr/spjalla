#include "haunted/core/key.h"

#include "core/client.h"
#include "core/input_line.h"
#include "core/plugin_host.h"

#include "plugins/plugin.h"

#include "formicine/ansi.h"

namespace spjalla::plugins {
	struct history_plugin: plugin {
		virtual ~history_plugin() {}

		std::string get_name() const override { return "History"; }
		std::string get_version() const override { return "0.0.0"; }

		int command_index = 0;
		size_t max_length = 4096;
		std::deque<std::string> input_history {};

		void startup(plugin_host *host) override {
			spjalla::client *client = dynamic_cast<spjalla::client *>(host);
			if (!client) {
				DBG("Error: expected client as plugin host");
				return;
			}

			client->handle_pre([&](const haunted::key &key, bool) {
				if (!input_history.empty()) {
					if (key == haunted::ktype::up_arrow && 0 < command_index) {
						client->get_ui().set_input(input_history[--command_index]);
						return cancelable_result::disable;
					}

					if (key == haunted::ktype::down_arrow && command_index < static_cast<int>(input_history.size()) - 1) {
						client->get_ui().set_input(input_history[++command_index]);
						return cancelable_result::disable;
					}
				}

				return cancelable_result::pass;
			});

			client->handle_post([&](const input_line &il) {
				input_history.push_back(il.original);

				if (max_length < input_history.size())
					input_history.pop_front();

				command_index = input_history.size();
			});
		}
	};
}

spjalla::plugins::history_plugin ext_plugin {};
