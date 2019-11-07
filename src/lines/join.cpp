#include "spjalla/core/client.h"
#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		return notice + parent->get_ui().render.nick(name, true) + " joined " +
			parent->get_ui().render.channel(chan_name);
	}
}
