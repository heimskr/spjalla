#ifndef SPJALLA_LINES_part_H_
#define SPJALLA_LINES_part_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/part.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct part_line: public line {
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user> user;
		const std::string name, reason;

		part_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_,
		const std::string &reason_, long stamp_):
			line(stamp_), chan(chan_), user(user_), name(user_->name), reason(reason_) {}

		part_line(const pingpong::part_event &ev): part_line(ev.chan, ev.who, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
