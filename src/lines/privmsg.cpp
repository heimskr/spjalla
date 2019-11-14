#include "pingpong/core/util.h"

#include "strender/strnode.h"

#include "spjalla/core/util.h"
#include "spjalla/lines/privmsg.h"
#include "spjalla/ui/renderer.h"

#include "lib/formicine/futil.h"

namespace spjalla::lines {
	notification_type privmsg_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string privmsg_line::render(ui::window *) {
		return parent->get_ui().render(is_action()? "action" : "privmsg", {
			{"raw_nick", name}, {"hats", hat_str()}, {"raw_message", message}
		});
	}

	// std::string privmsg_line::to_string(client *parent, const pingpong::privmsg_event &ev) {
	// 	ui::renderer &renderer = parent->get_ui().render;

	// 	strender::strnode &node = renderer.nodes.at(is_action()? "action" : "privmsg");
	// 	node = {
	// 		{"raw_nick", ev.speaker->name}, {"hats", ev.is_channel()? ev.get_channel()->get_hats(ev.speaker) : ""},
	// 		{"raw_message", ev.content}
	// 	};

	// 	return node.render();
	// }
}
