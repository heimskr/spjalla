#include "spjalla/core/client.h"
#include "spjalla/lines/part.h"

namespace spjalla::lines {
	std::string part_line::render(ui::window *) {
		strender::strnode &node = parent->get_ui().render.nodes.at("part");
		node = {{"raw_who", who}, {"raw_channel", chan->name}, {"raw_reason", reason}};
		return node.render();
	}
}
