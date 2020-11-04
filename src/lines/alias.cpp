#include "spjalla/lines/Alias.h"

namespace Spjalla::Lines {
	std::string AliasLine::render(UI::Window *) {
		return key + " = "_d + ansi::bold(expansion);
	}
}
