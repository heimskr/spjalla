#include "spjalla/core/client.h"
#include "spjalla/lines/kick.h"

namespace spjalla::lines {
	std::string kick_line::render(ui::window *) {
		strender::strnode &node = parent->get_ui().render.nodes.at(is_self? "kick_self" : "kick");
		node = {{"raw_who", who}, {"raw_whom", whom}, {"raw_channel", chan->name}, {"raw_reason", reason}};
		return node.render();
	}

	notification_type kick_line::get_notification_type() const {
		return is_self? notification_type::highlight : notification_type::info;
	}
}
