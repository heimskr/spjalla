#include "spjalla/lines/topic.h"

namespace spjalla::lines {
	topic_line::operator std::string() const {
		return lines::render_time(stamp) + lines::notice + ansi::bold(who) + " changed the topic of " +
			ansi::bold(where) + " to: " + topic;
	}
}
