#include "spjalla/lines/config_group.h"

namespace spjalla::lines {
	config_group_line::operator std::string() const {
		return lines::render_time(stamp) + "["_d + ansi::bold(group) + "]"_d;
	}
}
