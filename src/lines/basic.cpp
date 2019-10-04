#include "spjalla/lines/basic.h"

namespace spjalla::lines {
	basic_line::operator std::string() const {
		return lines::render_time(stamp) + text;
	}
}
