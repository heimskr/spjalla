#include "spjalla/core/client.h"
#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	std::string topic_line::render(ui::window *) {
		if (who.empty())
			return parent->get_ui().render("topic_is", {{"raw_channel", where}, {"raw_topic", topic}});
		return parent->get_ui().render("topic_change", {
			{"raw_who", who}, {"raw_channel", where}, {"raw_topic", topic}
		});
	}
}
