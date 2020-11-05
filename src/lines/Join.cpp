#include "spjalla/core/Client.h"
#include "spjalla/lines/Join.h"

namespace Spjalla::Lines {
	std::string JoinLine::render(UI::Window *) {
		return parent->getUI().renderer("join", {{"raw_who", name}, {"raw_channel", channelName}});
	}
}
