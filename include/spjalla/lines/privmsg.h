#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/privmsg.h"
#include "pingpong/events/privmsg.h"

#include "spjalla/lines/message.h"

namespace spjalla::lines {
	class privmsg_line: public message_line {
		public:
			using message_line::message_line;

			privmsg_line(client *parent_, const pingpong::privmsg_command &cmd, bool direct_only_ = false):
				privmsg_line(parent_, cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time, direct_only_) {}

			privmsg_line(client *parent_, const pingpong::privmsg_event &ev, bool direct_only_ = false):
				privmsg_line(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {}

			notification_type get_notification_type() const override;
			std::string get_format_key() const override;
	};
}

#endif
