#include "spjalla/lines/ConfigKey.h"

namespace Spjalla::Lines {
	std::string ConfigKeyLine::render(UI::Window *) {
		return (indent? "    " : "") + key + " = "_d + ansi::bold(std::string(value));
	}
}
