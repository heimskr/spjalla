#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	std::string topic_line::render(ui::window *) {
		if (who.empty())
			return lines::notice + "Topic for " + ansi::bold(where) + " is " + topic;

		return lines::notice + ansi::bold(who) + " changed the topic of " +
			ansi::bold(where) + " to: " + topic;
	}
}
