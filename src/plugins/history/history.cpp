#include "haunted/core/key.h"

#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/plugin_host.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	struct history_plugin: public plugin {
		~history_plugin() {}

		std::string get_name()        const override { return "History"; }
		std::string get_description() const override { return "Lets you repeat old inputs with the up and down keys."; }
		std::string get_version()     const override { return "0.1.0"; }

		int command_index = 0;
		size_t max_length = 4096;
		std::deque<std::string> input_history {};

		void postinit(plugin_host *host) override {
			spjalla::client *client = dynamic_cast<spjalla::client *>(host);
			if (!client) {
				DBG("Error: expected client as plugin host");
				return;
			}

			client->handle_pre(std::make_shared<plugin_host::pre_function<haunted::key>>(
			[&, client](const haunted::key &key, bool) {
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
			}));

			client->handle_post(std::make_shared<plugin_host::post_function<input_line>>([&](const input_line &il) {
				input_history.push_back(il.original);

				if (max_length < input_history.size())
					input_history.pop_front();

				command_index = input_history.size();
			}));
		}
	};
}

spjalla::plugins::history_plugin ext_plugin {};
