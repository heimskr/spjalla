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

#include "spjalla/plugins/nickcolor.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	config::validation_result nickcolor_plugin::validate_colorlist(const spjalla::config::value &val) {
		if (val.get_type() != config::value_type::string_)
			return config::validation_result::bad_type;

		std::vector<std::string> split = formicine::util::split(val.string_(), " ");
		if (split.empty())
			return config::validation_result::bad_value;

		for (const std::string &name: split) {
			if (!ansi::has_color(name))
				return config::validation_result::bad_value;
		}

		return config::validation_result::valid;
	}

	void nickcolor_plugin::preinit(PluginHost *host) {
		Spjalla::Client *client = dynamic_cast<Spjalla::Client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		config::RegisterKey("appearance", "nick_colors", "red orange yellow green blue magenta", validate_colorlist,
			[this](config::database &, const config::value &val) {
				std::vector<std::string> split = formicine::util::split(val.string_(), " ");
				colorlist.clear();
				for (const std::string &name: split)
					colorlist.push_back(ansi::get_color(name));
			}, "A list of colors to use for nick colorization.");
	}

	void nickcolor_plugin::postinit(PluginHost *) {
		parent->getUI().render["privmsg_nick"] = [this](strender::piece_map &pieces) -> std::string {
			const std::string raw = pieces.at("raw_nick").render();
			return ansi::wrap(raw, colorlist[std::hash<std::string>()(raw) % colorlist.size()]);
		};
	}

	void nickcolor_plugin::cleanup(PluginHost *) {
		config::unregister("appearance", "nick_colors");
		parent->getUI().render["privmsg_nick"] = parent->cache.format_nick_privmsg;
	}
}

spjalla::plugins::nickcolor_plugin ext_plugin {};
