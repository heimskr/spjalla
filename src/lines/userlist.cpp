#include "spjalla/lines/Userlist.h"

namespace Spjalla::Lines {
	std::string UserlistLine::render(UI::Window *) {
		const std::string hats = channel->getHats(user);
		const size_t hats_length = hats.length();
		return ansi::dim("- ") + (pad <= hats_length? "" : std::string(pad - hats_length, ' ')) + ansi::bold(hats)
			+ user->name;
	}
}
