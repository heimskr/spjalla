#include "spjalla/config/cache.h"
#include "spjalla/core/client.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {
	renderer::renderer(config::cache &cache_): cache(&cache_) {}

	void renderer::init_strnodes() {
		nodes["action"]  = {"action", cache->format_action};
		nodes["privmsg"] = {"privmsg", cache->format_privmsg};
		nodes["notice"]  = {"notice", cache->format_notice};
		nodes["channel"] = {"channel", cache->format_channel};
		nodes["reason"]  = {"reason", cache->format_reason};

		nodes["action_header"]  = {"header", cache->format_header_action,  &nodes["action"]};
		nodes["privmsg_header"] = {"header", cache->format_header_privmsg, &nodes["message"]};
		nodes["notice_header"]  = {"header", cache->format_header_notice,  &nodes["notice"]};

		nodes["action_message"]  = {"message", cache->format_message_action,  &nodes["action_header"]};
		nodes["privmsg_message"] = {"message", cache->format_message_privmsg, &nodes["privmsg_header"]};
		nodes["notice_message"]  = {"message", cache->format_message_notice,  &nodes["notice_header"]};

		nodes["action_nick"]  = {"nick", cache->format_nick_action,  &nodes["action_header"]};
		nodes["privmsg_nick"] = {"nick", cache->format_nick_privmsg, &nodes["privmsg_header"]};
		nodes["notice_nick"]  = {"nick", cache->format_nick_notice,  &nodes["notice_header"]};
	}

	void renderer::more_strnodes() {
		simple("quit", cache->format_quit);
		simple("kick", cache->format_kick);
		simple("join", cache->format_join);
		simple("part", cache->format_part);
		nodes["quit_reason"] = {"reason", cache->format_reason, &nodes["quit"]};
		nodes["kick_reason"] = {"reason", cache->format_reason, &nodes["kick"]};
		nodes["part_reason"] = {"reason", cache->format_reason, &nodes["part"]};
	}

	void renderer::simple(const char *name, const std::string &format) {
		nodes[name] = {name, format};
		strender::strnode &node = nodes[name];

		nodes[name + std::string("_who")]  = {"who",  formicine::util::replace_all(cache->format_nick_general,
			"$raw_nick$", "$raw_who$"),  &node};

		nodes[name + std::string("_whom")] = {"whom", formicine::util::replace_all(cache->format_nick_general,
			"$raw_nick$", "$raw_whom$"), &node};

		nodes[name + std::string("_channel")] = {"channel", cache->format_channel, &node};

		node = {{"-!-", lines::notice}, {"-!!-", lines::red_notice}, {"-!?-", lines::yellow_notice}};
	}

	std::string renderer::channel(const std::string &chan) {
		strender::strnode &node = nodes.at("channel");
		node = {{"raw_channel", chan}};
		return node.render();
	}
}
