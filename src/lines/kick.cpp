#include "spjalla/core/client.h"
#include "spjalla/lines/kick.h"

namespace spjalla::lines {
	std::string kick_line::render(ui::window *) {
		return parent->get_ui().render(is_self? "kick_self" : "kick", {
			{"raw_who", who}, {"raw_whom", whom}, {"raw_channel", chan->name}, {"raw_reason", reason}
		});
	}

	notification_type kick_line::get_notification_type() const {
		return is_self? notification_type::highlight : notification_type::info;
	}
}
