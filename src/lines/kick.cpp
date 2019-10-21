#include "spjalla/lines/kick.h"

namespace spjalla::lines {
	std::string kick_line::render(ui::window *) {
		if (is_self) {
			return red_notice + ansi::cyan(whom) + " was kicked from "
				+ ansi::bold(chan->name) + " by " + ansi::bold(who) + " ["_d + reason + "]"_d;
		} else {
			return notice + ansi::cyan(whom) + " was kicked from " +
				ansi::bold(chan->name) + " by " + ansi::bold(who) + " ["_d + reason + "]"_d;
		}
	}

	notification_type kick_line::get_notification_type() const {
		return is_self? notification_type::highlight : notification_type::info;
	}
}
