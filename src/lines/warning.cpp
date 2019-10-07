#include "spjalla/lines/warning.h"

namespace spjalla::lines {
	warning_line::operator std::string() const {
		return lines::render_time(stamp) + lines::yellow_notice + message;
	}
}
