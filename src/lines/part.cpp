#include "lines/part.h"

namespace spjalla::lines {
	part_line::operator std::string() const {
		return lines::render_time(stamp) + "-!- "_d + ansi::bold(name) + " left " + ansi::bold(chan->name) +
			" ["_d + reason + "]"_d;
	}
}
