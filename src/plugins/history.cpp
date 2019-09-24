#include "haunted/core/key.h"

#include "core/client.h"
#include "core/plugin_host.h"
#include "plugins/plugin.h"

#include "formicine/ansi.h"

namespace spjalla::plugins {
	struct history_plugin: plugin {
		virtual ~history_plugin() {}

		std::string get_name() const override { return "History"; }
		std::string get_version() const override { return "0.0.0"; }

		void startup(plugin_host *host) override {
			spjalla::client *client = dynamic_cast<spjalla::client *>(host);
			if (!client) {
				DBG("Error: expected client as plugin host");
				return;
			}

			client->handle_pre([&](const haunted::key &key, bool) {
				if (key == haunted::ktype::up_arrow) {
					DBG("up arrow");
					return cancelable_result::disable;
				} else if (key == haunted::ktype::down_arrow) {
					DBG("down arrow");
					return cancelable_result::disable;
				} else {
					return cancelable_result::pass;
				}
			});
		}
	};
}

spjalla::plugins::history_plugin ext_plugin {};
