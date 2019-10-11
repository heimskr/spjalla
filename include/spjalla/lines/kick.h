#ifndef SPJALLA_LINES_KICK_H_
#define SPJALLA_LINES_KICK_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/kick.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct kick_line: public line {
		std::shared_ptr<pingpong::channel> chan;
		const std::string who, whom, reason;
		bool is_self;

		kick_line(client *parent_, std::shared_ptr<pingpong::channel> chan_, const std::string &who_,
		const std::string &whom_, const std::string &reason_, long stamp_, bool is_self_ = false):
			line(parent_, stamp_), chan(chan_), who(who_), whom(whom_), reason(reason_), is_self(is_self_) {}

		kick_line(client *parent_, std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> who_,
		std::shared_ptr<pingpong::user> whom_, const std::string &reason_, long stamp_):
			kick_line(parent_, chan_, who_->name, whom_->name, reason_, stamp_, whom_->is_self()) {}

		kick_line(client *parent_, const pingpong::kick_event &ev):
			kick_line(parent_, ev.chan, ev.who, ev.whom, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override;
	};
}

#endif
