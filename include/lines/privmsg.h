#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "haunted/ui/textbox.h"

namespace spjalla::lines {
	class privmsg_line: haunted::ui::textline {
		pingpong::channel_ptr chan;
		pingpong::user_ptr user;
		const std::string message;

		privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_);

		virtual operator std::string() const override;
	};
}

#endif
