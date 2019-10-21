#include "pingpong/core/util.h"
#include "spjalla/lines/raw.h"

namespace spjalla::lines {
	std::string raw_line::render(ui::window *) {
		const char *indicator = is_out? "<< " : ">> ";
		const std::string styled = pingpong::util::irc2ansi(text);
		if (is_bad)
			return ansi::red(indicator) + styled;
		else if (is_out)
			return ansi::gray(indicator) + styled;
		return ansi::dim(ansi::gray(indicator)) + styled;
	}
}
