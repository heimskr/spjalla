#include "spjalla/core/Client.h"
#include "spjalla/lines/Error.h"

namespace Spjalla::Lines {
	std::string ErrorLine::render(UI::Window *) {
		return parent->getUI().renderer("bang_bad") + " " + message;
	}
}
