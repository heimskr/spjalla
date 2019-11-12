#include "spjalla/core/client.h"
#include "spjalla/lines/quit.h"

namespace spjalla::lines {
	std::string quit_line::render(ui::window *) {
		strender::strnode &node = parent->get_ui().render.nodes.at("quit");
		node = {{"raw_who", name}, {"raw_reason", message}};
		return node.render();
	}
}
