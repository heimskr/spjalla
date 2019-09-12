#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/commands/privmsg.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "haunted/ui/textbox.h"

namespace spjalla::lines {
	struct privmsg_line: haunted::ui::textline {
		pingpong::channel_ptr chan;
		pingpong::user_ptr user;
		const std::string message;
		long stamp;

		privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_, long stamp_);
		privmsg_line(const pingpong::privmsg_command &cmd):
			privmsg_line(cmd.destination, cmd.destination->serv->get_user(cmd.destination->serv->get_nick(), true),
			cmd.message, cmd.sent_time) {}

		virtual operator std::string() const override;
	};
}

#endif
