#include "core/sputil.h"
#include "ui/window.h"

namespace spjalla::util {
	std::string colorize_if_dead(const std::string &str, const ui::window *win) {
		return win->data.dead? ansi::red(str) : str;
	}
}
