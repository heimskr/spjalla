#include "spjalla/lines/error.h"

namespace spjalla::lines {
	error_line::operator std::string() const {
		return lines::render_time(stamp) + lines::red_notice + message;
	}
}
