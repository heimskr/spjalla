#include "spjalla/lines/Basic.h"

namespace Spjalla::Lines {
	std::string BasicLine::render(UI::Window *) {
		return text;
	}
}
