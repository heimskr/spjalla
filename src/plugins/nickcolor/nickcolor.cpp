#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "haunted/core/key.h"

#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/mode.h"
#include "pingpong/events/nick.h"
#include "pingpong/events/notice.h"
#include "pingpong/events/part.h"
#include "pingpong/events/privmsg.h"
#include "pingpong/events/quit.h"
#include "pingpong/events/topic.h"
#include "pingpong/events/topic_updated.h"

#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"

#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/plugin_host.h"
#include "spjalla/core/util.h"

#include "spjalla/lines/notice.h"
#include "spjalla/lines/privmsg.h"

#include "spjalla/plugins/nickcolor.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
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

	void nickcolor_plugin::preinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		config::register_key("appearance", "nick_colors", "red orange yellow green blue magenta", validate_colorlist,
			[this](config::database &, const config::value &val) {
				std::vector<std::string> split = formicine::util::split(val.string_(), " ");
				colorlist.clear();
				for (const std::string &name: split)
					colorlist.push_back(ansi::get_color(name));
			}, "A list of colors to use for nick colorization.");
	}

	void nickcolor_plugin::postinit(plugin_host *) {
		parent->get_ui().render["privmsg_nick"] = [this](strender::piece_map &pieces) -> std::string {
			const std::string raw = pieces.at("raw_nick").render();
			return ansi::wrap(raw, colorlist[std::hash<std::string>()(raw) % colorlist.size()]);
		};
	}

	void nickcolor_plugin::cleanup(plugin_host *) {
		config::unregister("appearance", "nick_colors");
		parent->get_ui().render["privmsg_nick"] = parent->cache.format_nick_privmsg;
	}
}

spjalla::plugins::nickcolor_plugin ext_plugin {};
