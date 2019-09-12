#include "lines/quit.h"

namespace spjalla::lines {
	quit_line::operator std::string() const {
		return lines::render_time(stamp) + ansi::wrap(user->name, ansi::color::cyan)
			+ " has quit " + "["_d + message + "]"_d;
	}
}
