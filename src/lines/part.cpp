#include "spjalla/lines/part.h"

namespace spjalla::lines {
	std::string part_line::render(ui::window *) {
		return notice + ansi::bold(name) + " left " + ansi::bold(chan_name) +
			" ["_d + reason + "]"_d;
	}
}
