#ifndef SPJALLA_CORE_CONFIGCACHE_H_
#define SPJALLA_CORE_CONFIGCACHE_H_

#include "spjalla/config/defaults.h"
#include "lib/formicine/ansi.h"

namespace spjalla {
	class configcache {
		private:
			static std::string get_string(const std::string &key);
			static ansi::color get_color(const std::string &key);
			static bool get_bool(const std::string &key);
			static long get_long(const std::string &key);
		public:
			configcache() {}

			ansi::color appearance_bar_background     = get_color("appearance.bar_background"),
			            appearance_bar_foreground     = get_color("appearance.bar_foreground"),
			            appearance_overlay_background = get_color("appearance.overlay_background"),
			            appearance_overlay_foreground = get_color("appearance.overlay_foreground"),
			            appearance_input_background   = get_color("appearance.input_background"),
			            appearance_input_foreground   = get_color("appearance.input_foreground"),
			            appearance_notice_foreground  = get_color("appearance.notice_foreground");
			
			bool behavior_answer_version_requests = get_bool("behavior.answer_version_requests"),
			     behavior_hide_version_requests   = get_bool("behavior.hide_version_requests");
			
			std::string completion_ping_suffix = get_string("completion.ping_suffix");

			bool debug_show_raw = get_bool("debug.show_raw");

			bool interface_close_on_part = get_bool("interface.close_on_part");
			long interface_scroll_buffer = get_long("interface.scroll_buffer");
			std::string interface_nick_format        = get_string("interface.nick_format"),
			            interface_nick_format_bright = get_string("interface.nick_format_bright"),
			            interface_channel_format     = get_string("interface.channel_format");

			bool messages_direct_only       = get_bool("messages.direct_only"),
			     messages_highlight_notices = get_bool("messages.highlight_notices"),
			     messages_notices_in_status = get_bool("messages.notices_in_status");

			std::string server_default_nick = get_string("server.default_nick"),
			            server_default_user = get_string("server.default_user"),
			            server_default_real = get_string("server.default_real");
	};
}

#endif
