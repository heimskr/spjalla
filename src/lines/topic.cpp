#include "spjalla/core/client.h"
#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	std::string topic_line::render(ui::window *) {
		if (who.empty()) {
			strender::strnode &node = parent->get_ui().render.nodes.at("topic_is");
			node = {{"raw_channel", where}, {"raw_topic", topic}};
			return node.render();
		}

		strender::strnode &node = parent->get_ui().render.nodes.at("topic_change");
		node = {{"raw_who", who}, {"raw_channel", where}, {"raw_topic", topic}};
		return node.render();
	}
}
