#include "spjalla/core/client.h"
#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		return parent->get_ui().render("join", {{"raw_who", name}, {"raw_channel", chan->name}});
	}
}
