#ifndef SPJALLA_LINES_JOIN_H_
#define SPJALLA_LINES_JOIN_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/events/join.h"

#include "lines/lines.h"

namespace spjalla::lines {
	class join_line: public haunted::ui::textline {
		private:
			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Removes the CTCP verb from the message. */
			std::string trimmed_message() const;

		public:
			pingpong::channel_ptr chan;
			pingpong::user_ptr user;
			long stamp;

			join_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, long stamp_):
				haunted::ui::textline(0), chan(chan_), user(user_), stamp(stamp_) {}

			join_line(const pingpong::join_event &ev): join_line(ev.chan, ev.who, ev.stamp) {}

			virtual operator std::string() const override;
	};
}

#endif
