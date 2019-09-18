#include "lines/chanlist.h"

namespace spjalla::lines {
	chanlist_line::operator std::string() const {
		return ansi::dim("- ") + static_cast<char>(chan->get_hat(user)) + chan->name;
	}
}
