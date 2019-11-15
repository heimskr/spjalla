#ifndef SPJALLA_CONFIG_CACHE_H_
#define SPJALLA_CONFIG_CACHE_H_

#include "spjalla/config/defaults.h"
#include "lib/formicine/ansi.h"

namespace spjalla::config {
	class cache {
		private:
			static std::string get_string(const std::string &key);
			static ansi::color get_color(const std::string &key);
			static bool get_bool(const std::string &key);
			static long get_long(const std::string &key);
		public:
			cache() {}

			ansi::color appearance_bar_background     = get_color("appearance.bar_background"),
			            appearance_bar_foreground     = get_color("appearance.bar_foreground"),
			            appearance_overlay_background = get_color("appearance.overlay_background"),
			            appearance_overlay_foreground = get_color("appearance.overlay_foreground"),
			            appearance_input_background   = get_color("appearance.input_background"),
			            appearance_input_foreground   = get_color("appearance.input_foreground"),
			            appearance_notice_foreground  = get_color("appearance.notice_foreground");
			bool appearance_allow_empty_hats = get_bool("appearance.allow_empty_hats");
			
			bool behavior_answer_version_requests = get_bool("behavior.answer_version_requests"),
			     behavior_hide_version_requests   = get_bool("behavior.hide_version_requests");
			
			std::string completion_ping_suffix = get_string("completion.ping_suffix");

			bool debug_show_raw = get_bool("debug.show_raw");

			bool interface_close_on_part = get_bool("interface.close_on_part"),
			     interface_show_motds    = get_bool("interface.show_motds");
			long interface_scroll_buffer = get_long("interface.scroll_buffer");

			bool messages_direct_only       = get_bool("messages.direct_only"),
			     messages_highlight_notices = get_bool("messages.highlight_notices"),
			     messages_notices_in_status = get_bool("messages.notices_in_status");

			std::string server_default_nick = get_string("server.default_nick"),
			            server_default_user = get_string("server.default_user"),
			            server_default_real = get_string("server.default_real");

#define DEF_FORMAT(n) std::string format_##n = get_string("format."#n);
			DEF_FORMAT(action);
			DEF_FORMAT(bang);
			DEF_FORMAT(bang_bad);
			DEF_FORMAT(bang_good);
			DEF_FORMAT(bang_warn);
			DEF_FORMAT(channel);
			DEF_FORMAT(header_action);
			DEF_FORMAT(header_privmsg);
			DEF_FORMAT(header_notice);
			DEF_FORMAT(join);
			DEF_FORMAT(kick);
			DEF_FORMAT(kick_self);
			DEF_FORMAT(message_action);
			DEF_FORMAT(message_privmsg);
			DEF_FORMAT(message_notice);
			DEF_FORMAT(nick_action);
			DEF_FORMAT(nick_change);
			DEF_FORMAT(nick_general);
			DEF_FORMAT(nick_general_bright);
			DEF_FORMAT(nick_privmsg);
			DEF_FORMAT(nick_notice);
			DEF_FORMAT(notice);
			DEF_FORMAT(part);
			DEF_FORMAT(privmsg);
			DEF_FORMAT(quit);
			DEF_FORMAT(reason);
			DEF_FORMAT(timestamp);
			DEF_FORMAT(topic);
			DEF_FORMAT(topic_is);
			DEF_FORMAT(topic_change);
#undef DEF_FORMAT
	};
}

#endif
