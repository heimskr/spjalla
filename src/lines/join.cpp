#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		return notice + ansi::bold(name) + " joined " + ansi::bold(chan_name);
	}
}
