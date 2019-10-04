#ifndef SPJALLA_LINES_JOIN_H_
#define SPJALLA_LINES_JOIN_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/join.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct join_line: public line {
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user> user;
		const std::string name;

		join_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_, long stamp_):
			line(stamp_), chan(chan_), user(user_), name(user_->name) {}

		join_line(const pingpong::join_event &ev):
			join_line(ev.chan, ev.who, ev.stamp) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
