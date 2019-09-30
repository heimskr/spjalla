#ifndef SPJALLA_LINES_KICK_H_
#define SPJALLA_LINES_KICK_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/kick.h"

#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	struct kick_line: public haunted::ui::textline {
		std::shared_ptr<pingpong::channel> chan;
		const std::string who;
		const std::string whom;
		const std::string reason;
		const long stamp;
		bool is_self;

		kick_line(std::shared_ptr<pingpong::channel> chan_, const std::string &who_, const std::string &whom_,
		const std::string &reason_, long stamp_, bool is_self_ = false):
			haunted::ui::textline(0), chan(chan_), who(who_), whom(whom_), reason(reason_), stamp(stamp_),
			is_self(is_self_) {}

		kick_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> who_,
		std::shared_ptr<pingpong::user> whom_, const std::string &reason_, long stamp_):
			kick_line(chan_, who_->name, whom_->name, reason_, stamp_, whom_->is_self()) {}

		kick_line(const pingpong::kick_event &ev): kick_line(ev.chan, ev.who, ev.whom, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
	};
}

#endif
