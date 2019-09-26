#include "lines/timed.h"

namespace spjalla::lines {
	timed_line::operator std::string() const {
		return lines::render_time(stamp) + text;
	}
}
