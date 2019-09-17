#include "lines/kick.h"

namespace spjalla::lines {
	kick_line::operator std::string() const {
		if (is_self) {
			return lines::render_time(stamp) + red_notice + ansi::cyan(whom) + " was kicked from "
				+ ansi::bold(chan->name) + " by " + ansi::bold(who) + " ["_d + reason + "]"_d;
		} else {
			return lines::render_time(stamp) + notice + ansi::cyan(whom) + " was kicked from " +
				ansi::bold(chan->name) + " by " + ansi::bold(who) + " ["_d + reason + "]"_d;
		}
	}
}
