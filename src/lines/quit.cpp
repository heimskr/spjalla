#include "spjalla/lines/quit.h"

namespace spjalla::lines {
	std::string quit_line::render(ui::window *) {
		return notice + ansi::cyan(name) + " has quit " + "["_d + message + "]"_d;
	}
}
