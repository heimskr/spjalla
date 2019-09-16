#ifndef SPJALLA_LINES_part_H_
#define SPJALLA_LINES_part_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/part.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct part_line: public haunted::ui::textline {
		pingpong::channel_ptr chan;
		pingpong::user_ptr user;
		const std::string name;
		const std::string reason;
		const long stamp;

		part_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &reason_, long stamp_):
			haunted::ui::textline(0), chan(chan_), user(user_), name(user_->name), reason(reason_), stamp(stamp_) {}

		part_line(const pingpong::part_event &ev): part_line(ev.chan, ev.who, ev.content, ev.stamp) {}

		virtual operator std::string() const override;
	};
}

#endif
