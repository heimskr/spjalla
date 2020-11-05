#include "spjalla/core/Client.h"
#include "spjalla/lines/Part.h"

namespace Spjalla::Lines {
	std::string PartLine::render(UI::Window *) {
		return parent->getUI().renderer("part", {
			{"raw_who", name}, {"raw_channel", channelName}, {"raw_reason", reason}
		});
	}
}
