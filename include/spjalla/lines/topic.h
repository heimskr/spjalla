#ifndef SPJALLA_LINES_TOPIC_H_
#define SPJALLA_LINES_TOPIC_H_

#include "pingpong/core/topicset.h"
#include "pingpong/events/topic.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct topic_line: public line {
		std::string who, where, topic;

		topic_line(client *parent_, const std::string &who_, const std::string &where_, const std::string &topic_, long stamp_):
			line(parent_, stamp_), who(who_), where(where_), topic(topic_) {}

		topic_line(client *parent_, const pingpong::topic_event &ev):
			topic_line(parent_, ev.who->name, ev.chan->name, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
