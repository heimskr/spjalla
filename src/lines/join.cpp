#include "spjalla/core/client.h"
#include "spjalla/lines/join.h"

namespace spjalla::lines {
	std::string join_line::render(ui::window *) {
		strender::strnode &node = parent->get_ui().render.nodes.at("join");
		node = {{"raw_who", who}, {"raw_channel", chan->name}};
		return node.render();
	}
}
