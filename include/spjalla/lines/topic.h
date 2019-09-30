#ifndef SPJALLA_LINES_TOPIC_H_
#define SPJALLA_LINES_TOPIC_H_

#include "pingpong/core/topicset.h"
#include "pingpong/events/topic.h"

#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	struct topic_line: public haunted::ui::textline {
		std::string who;
		std::string where;
		std::string topic;
		long stamp;

		topic_line(const std::string &who_, const std::string &where_, const std::string &topic_, long stamp_):
			haunted::ui::textline(0), who(who_), where(where_), topic(topic_), stamp(stamp_) {}

		topic_line(const pingpong::topic_event &ev): topic_line(ev.who->name, ev.chan->name, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
	};
}

#endif
