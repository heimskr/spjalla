#include "pingpong/core/Util.h"
#include "spjalla/lines/Raw.h"

namespace Spjalla::Lines {
	std::string RawLine::render(UI::Window *) {
		const char *indicator = isOut? "<< " : ">> ";
		const std::string styled = PingPong::Util::irc2ansi(text);
		if (isBad)
			return ansi::red(indicator) + styled;
		else if (isOut)
			return ansi::gray(indicator) + styled;
		return ansi::dim(ansi::gray(indicator)) + styled;
	}
}
