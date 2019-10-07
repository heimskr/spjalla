#include "spjalla/lines/userlist.h"

namespace spjalla::lines {
	userlist_line::operator std::string() const {
		const std::string hats = chan->get_hats(user);
		const size_t hats_length = hats.length();
		return ansi::dim("- ") + (pad <= hats_length? "" : std::string(pad - hats_length, ' ')) + hats + user->name;
	}
}
