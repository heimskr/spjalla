#ifndef SPJALLA_LINES_JOIN_H_
#define SPJALLA_LINES_JOIN_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/join.h"

#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	struct join_line: public haunted::ui::textline {
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user> user;
		const std::string name;
		const long stamp;

		join_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_, long stamp_):
			haunted::ui::textline(0), chan(chan_), user(user_), name(user_->name), stamp(stamp_) {}

		join_line(const pingpong::join_event &ev): join_line(ev.chan, ev.who, ev.stamp) {}

		virtual operator std::string() const override;
	};
}

#endif