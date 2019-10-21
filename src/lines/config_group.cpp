#include "spjalla/lines/config_group.h"

namespace spjalla::lines {
	std::string config_group_line::render(ui::window *) {
		return "["_d + ansi::bold(group) + "]"_d;
	}
}
