#include "lines/kick.h"

namespace spjalla::lines {
	kick_line::operator std::string() const {
		return lines::render_time(stamp) + "-!- "_d + ansi::cyan(whom) + " was kicked from " + ansi::bold(chan->name) +
			" by " + ansi::bold(who) + " ["_d + reason + "]"_d;
	}
}
