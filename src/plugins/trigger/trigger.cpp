#include <unordered_map>

#include "spjalla/plugins/trigger.h"

#include "pingpong/commands/kick.h"
#include "pingpong/core/util.h"
#include "pingpong/events/event.h"
#include "pingpong/events/mode.h"
#include "spjalla/core/client.h"
#include "spjalla/config/defaults.h"
#include "spjalla/lines/basic.h"
#include "spjalla/plugins/plugin.h"

#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	void trigger_plugin::preinit(plugin_host *host) {
		parent = dynamic_cast<spjalla::client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		config::register_key("trigger", "active", false, config::validate_bool,
			[&, this](config::database &, const config::value &value) {
				active = value.bool_();
			}, "Whether to react to +o events.");

		config::register_key("trigger", "reason", "Too slow! :^)", config::validate_string,
			[&, this](config::database &, const config::value &value) {
				reason = value.string_();
			}, "The message to use when auto-kicking someone.");
	}

	void trigger_plugin::postinit(plugin_host *) {
		pingpong::events::listen<pingpong::mode_event>("p:trigger", [=, this](pingpong::mode_event *ev) {
			if (!active)
				return;
			DBG("Mode string: " << ev->mset.mode_str() << ", name: " << ev->get_name());
			std::vector<std::string> split = util::split(ev->line.parameters, " ", false);
			if (split.size() < 3 || split[1] != "+oo")
				return;
			bool found = false;
			std::string other;
			for (int i = 2, l = split.size(); i < l; ++i) {
				if (split[i] == ev->serv->get_nick())
					found = true;
				else
					other = split[i];
			}

			if (!found)
				return;
			
			DBG("Other: " << other);
			pingpong::kick_command(ev->serv, ev->get_channel(ev->serv), other, reason).send();
		});
	}

	void trigger_plugin::cleanup(plugin_host *) {
		config::unregister("trigger", "active");
		config::unregister("trigger", "reason");
		pingpong::events::unlisten<pingpong::mode_event>("p:trigger");
	}
}

spjalla::plugins::trigger_plugin ext_plugin {};
