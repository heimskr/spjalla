#include "spjalla/lines/Chanlist.h"

namespace Spjalla::Lines {
	std::string ChanlistLine::render(UI::Window *) {
		return ansi::dim("- ") + std::string(channel->getHats(user)) + channel->name;
	}
}
