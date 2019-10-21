#include "spjalla/lines/chanlist.h"

namespace spjalla::lines {
	std::string chanlist_line::render(ui::window *) {
		return ansi::dim("- ") + std::string(chan->get_hats(user)) + chan->name;
	}

	chanlist_line::operator std::string() {
		return render(nullptr);
	}
}
