#include "spjalla/core/client.h"
#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		return notice + parent->get_ui().render.nick(name, chan_name, ui::renderer::nick_situation::normal, true)
			+ " joined " + parent->get_ui().render.channel(chan_name);
	}
}
