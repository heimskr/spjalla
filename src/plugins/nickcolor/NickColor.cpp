#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "haunted/core/Key.h"

#include "pingpong/events/Join.h"
#include "pingpong/events/Kick.h"
#include "pingpong/events/Mode.h"
#include "pingpong/events/Nick.h"
#include "pingpong/events/Notice.h"
#include "pingpong/events/Part.h"
#include "pingpong/events/Privmsg.h"
#include "pingpong/events/Quit.h"
#include "pingpong/events/Topic.h"
#include "pingpong/events/TopicUpdated.h"

#include "spjalla/config/Config.h"
#include "spjalla/config/Defaults.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/core/PluginHost.h"
#include "spjalla/core/Util.h"

#include "spjalla/lines/Notice.h"
#include "spjalla/lines/Privmsg.h"

#include "spjalla/plugins/NickColor.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	Config::ValidationResult NickColorPlugin::validateColorlist(const Spjalla::Config::Value &val) {
		if (val.getType() != Config::ValueType::String)
			return Config::ValidationResult::BadType;

		const std::vector<std::string> split = formicine::util::split(val.string_(), " ");
		if (split.empty())
			return Config::ValidationResult::BadValue;

		for (const std::string &name: split)
			if (!ansi::has_color(name))
				return Config::ValidationResult::BadValue;

		return Config::ValidationResult::Valid;
	}

	void NickColorPlugin::preinit(PluginHost *host) {
		Spjalla::Client *client = dynamic_cast<Spjalla::Client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		Config::RegisterKey("appearance", "nick_colors", "red orange yellow green blue magenta", validateColorlist,
			[this](Config::Database &, const Config::Value &val) {
				std::vector<std::string> split = formicine::util::split(val.string_(), " ");
				colorlist.clear();
				for (const std::string &name: split)
					colorlist.push_back(ansi::get_color(name));
			}, "A list of colors to use for nick colorization.");
		Config::registered.at("appearance.nick_colors").apply(parent->configs,
			parent->configs.get("appearance", "nick_colors"));
	}

	void NickColorPlugin::postinit(PluginHost *) {
		parent->getUI().renderer["privmsg_nick"] = [this](Strender::PieceMap &pieces) -> std::string {
			const std::string raw = pieces.at("raw_nick").render();
			if (colorlist.empty())
				return raw;
			return ansi::wrap(raw, colorlist[std::hash<std::string>()(raw) % colorlist.size()]);
		};
	}

	void NickColorPlugin::cleanup(PluginHost *) {
		Config::unregister("appearance", "nick_colors");
		parent->getUI().renderer["privmsg_nick"] = parent->cache.formatNickPrivmsg;
	}
}

Spjalla::Plugins::NickColorPlugin ext_plugin {};
