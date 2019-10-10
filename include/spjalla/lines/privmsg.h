#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/privmsg.h"
#include "pingpong/events/privmsg.h"

#include "spjalla/lines/message.h"

namespace spjalla::lines {
	struct privmsg_format { static constexpr const char *message = "^d<^D%h%s^d>^D %m", *action = "^b*^B %h%s %m"; };

	class privmsg_line: public message_line<privmsg_format> {
		public:
			using message_line::message_line;

			privmsg_line(const pingpong::privmsg_command &cmd, bool direct_only_ = false):
				privmsg_line(cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time, direct_only_) {}

			privmsg_line(const pingpong::privmsg_event &ev, bool direct_only_ = false):
				privmsg_line(ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {}

			virtual notification_type get_notification_type() const override;

			static std::string to_string(const pingpong::privmsg_event &, bool with_time = true);
	};
}

#endif
