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
		std::deque<std::string> history {};

		std::shared_ptr<plugin_host::pre_function<haunted::key>> prekey = 
			std::make_shared<plugin_host::pre_function<haunted::key>>([this](const haunted::key &key, bool) {
				if (!history.empty()) {
					if (key == haunted::ktype::up_arrow && 0 < command_index) {
						parent->get_ui().set_input(history[--command_index]);
						return cancelable_result::disable;
					}

					if (key == haunted::ktype::down_arrow && command_index < static_cast<int>(history.size()) - 1) {
						parent->get_ui().set_input(history[++command_index]);
						return cancelable_result::disable;
					}
				}

				return cancelable_result::pass;
			});

		std::shared_ptr<plugin_host::post_function<input_line>> postinput =
			std::make_shared<plugin_host::post_function<input_line>>([this](const input_line &il) {
				history.push_back(il.original);

				if (max_length < history.size())
					history.pop_front();

				command_index = history.size();
			});

		void postinit(plugin_host *host) override {
			parent = dynamic_cast<spjalla::client *>(host);
			if (!parent) {
				DBG("Error: expected client as plugin host");
				return;
			}

			parent->handle(prekey);
			parent->handle(postinput);
		}

		void cleanup(plugin_host *) override {
			parent->unhandle(prekey);
			parent->unhandle(postinput);
		}
	};
}

spjalla::plugins::history_plugin ext_plugin {};
