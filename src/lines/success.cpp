#include "spjalla/core/Client.h"
#include "spjalla/lines/Success.h"

namespace Spjalla::Lines {
	std::string SuccessLine::render(UI::Window *) {
		return parent->getUI().renderer("bang_good") + " " + message;
	}
}
