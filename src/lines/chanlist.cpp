#include "spjalla/lines/chanlist.h"

namespace spjalla::lines {
	chanlist_line::operator std::string() const {
		return ansi::dim("- ") + std::string(chan->get_hats(user)) + chan->name;
	}
}
