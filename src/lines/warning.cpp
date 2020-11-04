#include "spjalla/core/Client.h"
#include "spjalla/lines/Warning.h"

namespace Spjalla::Lines {
	std::string WarningLine::render(UI::Window *) {
		return parent->getUI().renderer("bang_warn") + " " + message;
	}
}
