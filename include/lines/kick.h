#ifndef SPJALLA_LINES_KICK_H_
#define SPJALLA_LINES_KICK_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/kick.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct kick_line: public haunted::ui::textline {
		pingpong::channel_ptr chan;
		const std::string who;
		const std::string whom;
		const std::string reason;
		const long stamp;

		kick_line(pingpong::channel_ptr chan_, const std::string &who_, const std::string &whom_,
		const std::string &reason_, long stamp_):
			haunted::ui::textline(0), chan(chan_), who(who_), whom(whom_), reason(reason_), stamp(stamp_) {}

		kick_line(pingpong::channel_ptr chan_, pingpong::user_ptr who_, pingpong::user_ptr whom_,
		const std::string &reason_, long stamp_):
			kick_line(chan_, who_->name, whom_->name, reason_, stamp_) {}

		kick_line(const pingpong::kick_event &ev): kick_line(ev.chan, ev.who, ev.whom, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
	};
}

#endif
