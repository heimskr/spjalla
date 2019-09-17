#include "lines/join.h"

namespace spjalla::lines {
	join_line::operator std::string() const {
		return lines::render_time(stamp) + notice + ansi::bold(name) + " joined " + ansi::bold(chan->name);
	}
}
