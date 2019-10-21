#include "spjalla/lines/raw.h"

namespace spjalla::lines {
	std::string raw_line::render(ui::window *) {
		const char *indicator = is_out? "<< " : ">> ";
		if (is_bad)
			return ansi::red(indicator) + text;
		else if (is_out)
			return ansi::gray(indicator) + text;
		return ansi::dim(ansi::gray(indicator)) + text;
	}
}
