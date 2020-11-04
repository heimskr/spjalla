#include <unordered_map>

#include "spjalla/plugins/trigger.h"

#include "pingpong/commands/kick.h"
#include "pingpong/core/Util.h"
#include "pingpong/events/Event.h"
#include "pingpong/events/Mode.h"
#include "spjalla/core/Client.h"
#include "spjalla/config/Defaults.h"
#include "spjalla/lines/basic.h"
#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins {
	void trigger_plugin::preinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		config::RegisterKey("trigger", "active", false, config::validateBool,
			[&, this](config::database &, const config::value &value) {
				active = value.bool_();
			}, "Whether to react to +o events.");

		config::RegisterKey("trigger", "reason", "Too slow! :^)", config::validate_string,
			[&, this](config::database &, const config::value &value) {
				reason = value.string_();
			}, "The message to use when auto-kicking someone.");
	}

	void trigger_plugin::postinit(PluginHost *) {
		PingPong::Events::listen<PingPong::mode_event>("p:trigger", [=, this](PingPong::mode_event *ev) {
			if (!active)
				return;
			DBG("Mode string: " << ev->mset.mode_str() << ", name: " << ev->get_name());
			std::vector<std::string> split = util::split(ev->line.parameters, " ", false);
			if (split.size() < 3 || split[1] != "+oo")
				return;
			bool found = false;
			std::string other;
			for (int i = 2, l = split.size(); i < l; ++i) {
				if (split[i] == ev->server->get_nick())
					found = true;
				else
					other = split[i];
			}

			if (!found)
				return;
			
			DBG("Other: " << other);
			PingPong::kick_command(ev->serv, ev->getChannel(ev->serv), other, reason).send();
		});
	}

	void trigger_plugin::cleanup(PluginHost *) {
		config::unregister("trigger", "active");
		config::unregister("trigger", "reason");
		PingPong::Events::unlisten<PingPong::mode_event>("p:trigger");
	}
}

spjalla::plugins::trigger_plugin ext_plugin {};
