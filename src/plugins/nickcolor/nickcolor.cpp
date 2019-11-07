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

		return val.string_().find_first_not_of("0123456789 ") == std::string::npos?
			config::validation_result::valid : config::validation_result::bad_value;
	}

	void nickcolor_plugin::apply_colorlist(config::database &, const config::value &) {

	}

	void nickcolor_plugin::preinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		config::register_key("appearance", "nick_colors", true, validate_colorlist, apply_colorlist,
			"A list of colors to use for nick colorization.");
	}

	void nickcolor_plugin::postinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }

		client->get_ui().render.provide_nick(
			[client](std::string format, const std::string &nick, const std::string &,
			         ui::renderer::nick_situation situation) -> std::string {
				if (situation == ui::renderer::nick_situation::normal)
					return client->get_ui().render.nick_impl(format, nick);
				// return "\e[39m" + nick;
				return nick;
			});
	}
}

spjalla::plugins::nickcolor_plugin ext_plugin {};
