#include "spjalla/lines/nick_change.h"

namespace spjalla::lines {
	nick_change_line::operator std::string() const {
		return lines::render_time(stamp) + notice + ansi::cyan(old_name) + " is now known as " + ansi::cyan(new_name);
	}
}
