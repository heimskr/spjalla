#include "spjalla/lines/nick_change.h"

namespace spjalla::lines {
	std::string nick_change_line::render(ui::window *) {
		return notice + ansi::cyan(old_name) + " is now known as " + ansi::cyan(new_name);
	}
}
