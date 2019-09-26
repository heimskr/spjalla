#include "core/sputil.h"
#include "ui/window.h"

namespace spjalla {
	std::string util::colorize_if_dead(const std::string &str, const ui::window *win) {
		return win->data.dead? ansi::red(str) : str;
	}
}
