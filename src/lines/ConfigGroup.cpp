#include "spjalla/lines/ConfigGroup.h"

namespace Spjalla::Lines {
	std::string ConfigGroupLine::render(UI::Window *) {
		return "["_d + ansi::bold(group) + "]"_d;
	}
}
