#include "spjalla/core/client.h"
#include "spjalla/lines/part.h"

namespace spjalla::lines {
	std::string part_line::render(ui::window *) {
		return notice + parent->get_ui().format_nick(name, false) + " left " +
			parent->get_ui().format_channel(chan_name) + " ["_d + reason + "]"_d;
	}
}
