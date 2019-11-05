#include "spjalla/core/client.h"
#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		return notice + parent->get_ui().format_nick(name, true) + " joined " +
			parent->get_ui().format_channel(chan_name);
	}
}
