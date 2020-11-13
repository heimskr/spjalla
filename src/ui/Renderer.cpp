#include "spjalla/config/Cache.h"
#include "spjalla/core/Client.h"
#include "spjalla/ui/Renderer.h"

namespace Spjalla::UI {
	Renderer::Renderer(Config::Cache &cache_):
		cache(&cache_) {}

	std::string Renderer::operator()(const std::string &key, const Strender::PieceMap &pieces) {
		return nodes.at(key)->render(pieces);
	}

	std::string Renderer::operator()(const std::string &key) {
		return nodes.at(key)->render();
	}

	Strender::StrNode & Renderer::operator[](const std::string &key) {
		return *nodes.at(key);
	}

	void Renderer::initStrNodes() {
		insert("action",    "action",    cache->formatAction);
		insert("privmsg",   "privmsg",   cache->formatPrivmsg);
		insert("notice",    "notice",    cache->formatNotice);
		insert("channel",   "channel",   cache->formatChannel);
		insert("reason",    "reason",    cache->formatReason);
		insert("timestamp", "timestamp", cache->formatTimestamp);

		insert("action_header",  "header", cache->formatHeaderAction,  &*nodes.at("action"));
		insert("privmsg_header", "header", cache->formatHeaderPrivmsg, &*nodes.at("privmsg"));
		insert("notice_header",  "header", cache->formatHeaderNotice,  &*nodes.at("notice"));

		insert("action_message",  "message", cache->formatMessageAction,  &*nodes.at("action_header"));
		insert("privmsg_message", "message", cache->formatMessagePrivmsg, &*nodes.at("privmsg_header"));
		insert("notice_message",  "message", cache->formatMessageNotice,  &*nodes.at("notice_header"));

		insert("action_nick",  "nick", cache->formatNickAction,  &*nodes.at("action_header"));
		insert("privmsg_nick", "nick", cache->formatNickPrivmsg, &*nodes.at("privmsg_header"));
		insert("notice_nick",  "nick", cache->formatNickNotice,  &*nodes.at("notice_header"));

		insert("nick_general", "nick", cache->formatNickGeneral);
		insert("nick_general_bright", "nick", cache->formatNickGeneralBright);

		insert("bang",      "!",     cache->formatBang);
		insert("bang_bad",  "bad!",  cache->formatBangBad);
		insert("bang_good", "good!", cache->formatBangGood);
		insert("bang_warn", "warn!", cache->formatBangWarn);
	}

	std::string Renderer::bang() {
		return (*this)("!");
	}

	std::string Renderer::bad() {
		return (*this)("bad!");
	}

	std::string Renderer::good() {
		return (*this)("good!");
	}

	std::string Renderer::warn() {
		return (*this)("warn!");
	}

	void Renderer::moreStrNodes() {
		simple("quit", cache->formatQuit);
		simple("kick", cache->formatKick);
		simple("kick_self", cache->formatKickSelf);
		simple("join", cache->formatJoin);
		simple("part", cache->formatPart);
		insert("quit_reason", "reason", cache->formatReason, &*nodes.at("quit"));
		insert("kick_reason", "reason", cache->formatReason, &*nodes.at("kick"));
		insert("kick_self_reason", "reason", cache->formatReason, &*nodes.at("kick_self"));
		insert("part_reason", "reason", cache->formatReason, &*nodes.at("part"));
		insert("nick_change", "nick_change", cache->formatNickChange);
		Strender::StrNode &nick_change = *nodes.at("nick_change");
		insert("nick_change_new", "new", replaceNick("$raw_new$"), &nick_change);
		insert("nick_change_old", "old", replaceNick("$raw_old$"), &nick_change);
		insert("self_nick_change", "self_nick_change", cache->formatSelfNickChange);
		Strender::StrNode &self_nick_change = *nodes.at("self_nick_change");
		insert("self_nick_change_new", "new", replaceNick("$raw_new$"), &self_nick_change);
	}

	std::string Renderer::replaceNick(const std::string &varname) const {
		return formicine::util::replace_all(cache->formatNickGeneral, "$raw_nick$", varname);
	}

	void Renderer::simple(const std::string &name, const std::string &format) {
		insert(name, name.c_str(), format);
		Strender::StrNode &node = *nodes.at(name);
		insert(name + "_who",     "who",     replaceNick("$raw_who$"),  &node);
		insert(name + "_whom",    "whom",    replaceNick("$raw_whom$"), &node);
		insert(name + "_channel", "channel", cache->formatChannel,      &node);
		insert(name + "_bang",      "!",     cache->formatBang,      &node);
		insert(name + "_bang_bad",  "bad!",  cache->formatBangBad,  &node);
		insert(name + "_bang_good", "good!", cache->formatBangGood, &node);
		insert(name + "_bang_warn", "warn!", cache->formatBangWarn, &node);
		// node = {{"-!-"s, lines::notice}, {"-!!-"s, lines::red_notice}, {"-!?-"s, lines::yellow_notice}};
	}

	std::string Renderer::channel(const std::string &chan) {
		return nodes.at("channel")->render({{std::string("raw_channel"), chan}});
	}

	std::string Renderer::nick(const std::string &nick, bool bright) {
		return nodes.at(bright? "nick_general" : "nick_general_bright")->render({{std::string("raw_nick"), nick}});
	}
}
