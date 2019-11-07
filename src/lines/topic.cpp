#include "spjalla/core/client.h"
#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	std::string topic_line::render(ui::window *) {
		if (who.empty())
			return lines::notice + "Topic for " + parent->get_ui().render.channel(where) + " is " + topic;

		return lines::notice + parent->get_ui().render.nick(who, where, ui::renderer::nick_situation::normal, true)
			+ " changed the topic of " + parent->get_ui().render.channel(where) + " to: " + topic;
	}
}
