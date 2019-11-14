#include "spjalla/config/cache.h"
#include "spjalla/core/client.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {
	renderer::renderer(config::cache &cache_): cache(&cache_) {}

	std::string renderer::operator()(const std::string &key, const strender::piece_map &pieces) {
		return nodes.at(key)->render(pieces);
	}

	std::string renderer::operator()(const std::string &key) {
		return nodes.at(key)->render();
	}

	strender::strnode & renderer::operator[](const std::string &key) {
		return *nodes.at(key);
	}

	void renderer::init_strnodes() {
		insert("action", "action", cache->format_action);
		insert("privmsg",   "privmsg",   cache->format_privmsg);
		insert("notice",    "notice",    cache->format_notice);
		insert("channel",   "channel",   cache->format_channel);
		insert("reason",    "reason",    cache->format_reason);
		insert("timestamp", "timestamp", cache->format_reason);

		insert("action_header",  "header", cache->format_header_action,  &*nodes.at("action"));
		insert("privmsg_header", "header", cache->format_header_privmsg, &*nodes.at("privmsg"));
		insert("notice_header",  "header", cache->format_header_notice,  &*nodes.at("notice"));

		insert("action_message",  "message", cache->format_message_action,  &*nodes.at("action_header"));
		insert("privmsg_message", "message", cache->format_message_privmsg, &*nodes.at("privmsg_header"));
		insert("notice_message",  "message", cache->format_message_notice,  &*nodes.at("notice_header"));

		insert("action_nick",  "nick", cache->format_nick_action,  &*nodes.at("action_header"));
		insert("privmsg_nick", "nick", cache->format_nick_privmsg, &*nodes.at("privmsg_header"));
		insert("notice_nick",  "nick", cache->format_nick_notice,  &*nodes.at("notice_header"));
	}

	void renderer::more_strnodes() {
		simple("quit", cache->format_quit);
		simple("kick", cache->format_kick);
		simple("join", cache->format_join);
		simple("part", cache->format_part);
		insert("quit_reason", "reason", cache->format_reason, &*nodes.at("quit"));
		insert("kick_reason", "reason", cache->format_reason, &*nodes.at("kick"));
		insert("part_reason", "reason", cache->format_reason, &*nodes.at("part"));
		insert("nick_change", "nick_change", cache->format_nick_change);
		strender::strnode &nick_change = *nodes.at("nick_change");
		insert("nick_change_new", "new", replace_nick("$raw_new$"), &nick_change);
		insert("nick_change_old", "old", replace_nick("$raw_old$"), &nick_change);
	}

	std::string renderer::replace_nick(const std::string &varname) const {
		return formicine::util::replace_all(cache->format_nick_general, "$raw_nick$", varname);
	}

	void renderer::simple(const char *name, const std::string &format) {
		insert(name, name, format);
		strender::strnode &node = *nodes.at(name);
		insert(name + std::string("_who"),     "who",     replace_nick("$raw_who$"),  &node);
		insert(name + std::string("_whom"),    "whom",    replace_nick("$raw_whom$"), &node);
		insert(name + std::string("_channel"), "channel", cache->format_channel,      &node);
		node = {{"-!-", lines::notice}, {"-!!-", lines::red_notice}, {"-!?-", lines::yellow_notice}};
	}

	std::string renderer::channel(const std::string &chan) {
		return nodes.at("channel")->render({{"raw_channel", chan}});
	}

	std::string renderer::nick(const std::string &nick, bool bright) {
		strender::strnode &node = *nodes.at(bright? "nick_general" : "nick_general_bright");
		node = {{"raw_nick", nick}};
		return node.render();
	}
}
