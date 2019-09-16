#include "lines/userlist.h"

namespace spjalla::lines {
	userlist_line::operator std::string() const {
		return ansi::dim("- ") + static_cast<char>(chan->get_hat(user)) + user->name;
	}
}
