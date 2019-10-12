#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	topic_line::operator std::string() const {
		if (who.empty())
			return lines::render_time(stamp) + lines::notice + "Topic for " + ansi::bold(where) + " is " + topic;

		return lines::render_time(stamp) + lines::notice + ansi::bold(who) + " changed the topic of " +
			ansi::bold(where) + " to: " + topic;
	}
}
