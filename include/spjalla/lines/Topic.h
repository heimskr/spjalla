#ifndef SPJALLA_LINES_TOPIC_H_
#define SPJALLA_LINES_TOPIC_H_

#include "pingpong/core/TopicSet.h"
#include "pingpong/events/Topic.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct TopicLine: public Line {
		std::string who, where, topic;

		TopicLine(Client *parent_, const std::string &who_, const std::string &where_, const std::string &topic_,
		long stamp_):
			Line(parent_, stamp_), who(who_), where(where_), topic(topic_) {}

		TopicLine(Client *parent_, const PingPong::TopicEvent &ev):
			TopicLine(parent_, ev.who->name, ev.channel->name, ev.content, ev.stamp) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
