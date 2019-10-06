#include "spjalla/lines/userlist.h"

namespace spjalla::lines {
	userlist_line::operator std::string() const {
		return ansi::dim("- ") + std::string(chan->get_hats(user)) + user->name;
	}
}
