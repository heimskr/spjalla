#include <unordered_map>

#include "spjalla/plugins/Trigger.h"

#include "pingpong/commands/Kick.h"
#include "pingpong/core/Util.h"
#include "pingpong/events/Event.h"
#include "pingpong/events/Mode.h"
#include "spjalla/core/Client.h"
#include "spjalla/config/Defaults.h"
#include "spjalla/lines/Basic.h"
#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins {
	void TriggerPlugin::preinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		Config::RegisterKey("trigger", "active", false, Config::validateBool,
			[&, this](Config::Database &, const Config::Value &value) {
				active = value.bool_();
			}, "Whether to react to +o events.");

		Config::RegisterKey("trigger", "reason", "Too slow! :^)", Config::validateString,
			[&, this](Config::Database &, const Config::Value &value) {
				reason = value.string_();
			}, "The message to use when auto-kicking someone.");
	}

	void TriggerPlugin::postinit(PluginHost *) {
		PingPong::Events::listen<PingPong::ModeEvent>("p:trigger", [=, this](PingPong::ModeEvent *ev) {
			if (!active)
				return;
			DBG("Mode string: " << ev->modeSet.modeString() << ", name: " << ev->getName());
			std::vector<std::string> split = Util::split(ev->line.parameters, " ", false);
			if (split.size() < 3 || split[1] != "+oo")
				return;
			bool found = false;
			std::string other;
			for (int i = 2, l = split.size(); i < l; ++i) {
				if (split[i] == ev->server->getNick())
					found = true;
				else
					other = split[i];
			}

			if (!found)
				return;
			
			DBG("Other: " << other);
			PingPong::KickCommand(ev->server, ev->getChannel(ev->server), other, reason).send();
		});
	}

	void TriggerPlugin::cleanup(PluginHost *) {
		Config::unregister("trigger", "active");
		Config::unregister("trigger", "reason");
		PingPong::Events::unlisten<PingPong::ModeEvent>("p:trigger");
	}
}

Spjalla::Plugins::TriggerPlugin ext_plugin {};
