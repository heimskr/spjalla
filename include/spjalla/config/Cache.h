#ifndef SPJALLA_CONFIG_CACHE_H_
#define SPJALLA_CONFIG_CACHE_H_

#include "spjalla/config/Defaults.h"
#include "lib/formicine/ansi.h"

namespace Spjalla::Config {
	class Cache {
		private:
			static std::string getString(const std::string &key);
			static ansi::color getColor(const std::string &key);
			static bool getBool(const std::string &key);
			static long getLong(const std::string &key);
		public:
			Cache() {}

			ansi::color appearanceBarBackground     = getColor("appearance.bar_background"),
			            appearanceBarForeground     = getColor("appearance.bar_foreground"),
			            appearanceOverlayBackground = getColor("appearance.overlay_background"),
			            appearanceOverlayForeground = getColor("appearance.overlay_foreground"),
			            appearanceInputBackground   = getColor("appearance.input_background"),
			            appearanceInputForeground   = getColor("appearance.input_foreground"),
			            appearanceNoticeForeground  = getColor("appearance.notice_foreground");
			bool appearanceAllowEmptyHats = getBool("appearance.allow_empty_hats");
			
			bool behaviorAnswerVersionRequests = getBool("behavior.answer_version_requests"),
			     behaviorHideVersionRequests   = getBool("behavior.hide_version_requests");
			
			std::string completionPingSuffix = getString("completion.ping_suffix");

			bool debugShowRaw = getBool("debug.show_raw");

			bool interfaceCloseOnPart  = getBool("interface.close_on_part"),
			     interfaceShowMotds    = getBool("interface.show_motds");
			long interfaceScrollBuffer = getLong("interface.scroll_buffer");

			bool messagesDirectOnly       = getBool("messages.direct_only"),
			     messagesHighlightNotices = getBool("messages.highlight_notices"),
			     messagesNoticesInStatus  = getBool("messages.notices_in_status");

			std::string serverDefaultNick = getString("server.default_nick"),
			            serverDefaultUser = getString("server.default_user"),
			            serverDefaultReal = getString("server.default_real");

#define DEF_FORMAT(n, u) std::string format##n = getString("format."#u);
			DEF_FORMAT(Action, action);
			DEF_FORMAT(Bang, bang);
			DEF_FORMAT(BangBad, bang_bad);
			DEF_FORMAT(BangGood, bang_good);
			DEF_FORMAT(BangWarn, bang_warn);
			DEF_FORMAT(Channel, channel);
			DEF_FORMAT(HeaderAction, header_action);
			DEF_FORMAT(HeaderPrivmsg, header_privmsg);
			DEF_FORMAT(HeaderNotice, header_notice);
			DEF_FORMAT(Join, join);
			DEF_FORMAT(Kick, kick);
			DEF_FORMAT(KickSelf, kick_self);
			DEF_FORMAT(MessageAction, message_action);
			DEF_FORMAT(MessagePrivmsg, message_privmsg);
			DEF_FORMAT(MessageNotice, message_notice);
			DEF_FORMAT(NickAction, nick_action);
			DEF_FORMAT(NickChange, nick_change);
			DEF_FORMAT(NickGeneral, nick_general);
			DEF_FORMAT(NickGeneralBright, nick_general_bright);
			DEF_FORMAT(NickPrivmsg, nick_privmsg);
			DEF_FORMAT(NickNotice, nick_notice);
			DEF_FORMAT(Notice, notice);
			DEF_FORMAT(Part, part);
			DEF_FORMAT(Privmsg, privmsg);
			DEF_FORMAT(Quit, quit);
			DEF_FORMAT(Reason, reason);
			DEF_FORMAT(Timestamp, timestamp);
			DEF_FORMAT(Topic, topic);
			DEF_FORMAT(TopicIs, topic_is);
			DEF_FORMAT(TopicChange, topic_change);
#undef DEF_FORMAT
	};
}

#endif
