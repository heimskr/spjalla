#include "spjalla/core/Client.h"
#include "spjalla/lines/Quit.h"

namespace Spjalla::Lines {
	std::string QuitLine::render(UI::Window *) {
		return parent->getUI().renderer("quit", {{"raw_who", name}, {"raw_reason", message}});
	}
}
