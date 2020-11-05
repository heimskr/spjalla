#include "spjalla/core/Client.h"
#include "spjalla/lines/Topic.h"

namespace Spjalla::Lines {
	std::string TopicLine::render(UI::Window *) {
		if (who.empty())
			return parent->getUI().renderer("topic_is", {{"raw_channel", where}, {"raw_topic", topic}});
		return parent->getUI().renderer("topic_change", {
			{"raw_who", who}, {"raw_channel", where}, {"raw_topic", topic}
		});
	}
}
