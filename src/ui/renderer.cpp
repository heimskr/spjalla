#include "spjalla/config/cache.h"
#include "spjalla/core/client.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {
	renderer::renderer(config::cache &cache_): cache(&cache_) {}

	void renderer::init_strnodes() {
		action  = strender::strnode("*", cache->format_action);
		message = strender::strnode("*", cache->format_privmsg);
		notice  = strender::strnode("*", cache->format_notice);

		action_header  = strender::strnode("header", cache->format_header_action,  &action);
		privmsg_header = strender::strnode("header", cache->format_header_privmsg, &message);
		notice_header  = strender::strnode("header", cache->format_header_notice,  &notice);

		action_message  = strender::strnode("message", cache->format_message_action,  &action_header);
		privmsg_message = strender::strnode("message", cache->format_message_privmsg, &privmsg_header);
		notice_message  = strender::strnode("message", cache->format_message_notice,  &notice_header);

		action_nick  = strender::strnode("nick", cache->format_nick_action,  &action_header);
		privmsg_nick = strender::strnode("nick", cache->format_nick_privmsg, &privmsg_header);
		notice_nick  = strender::strnode("nick", cache->format_nick_notice,  &notice_header);

		quit = strender::strnode("*", cache->format_quit);
		kick = strender::strnode("*", cache->format_kick);
		join = strender::strnode("*", cache->format_join);
		part = strender::strnode("*", cache->format_part);
	}

	void renderer::copy_strnodes() {

	}
}
