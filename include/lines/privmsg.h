#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/commands/privmsg.h"
#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "lines/lines.h"

namespace spjalla::lines {
	class privmsg_line: public haunted::ui::textline {
		private:
			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Removes the CTCP verb from the message. */
			std::string trimmed_message() const;

		public:
			pingpong::channel_ptr chan;
			pingpong::user_ptr user;
			const std::string message;
			long stamp;

			privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_, long stamp_);
			privmsg_line(const pingpong::privmsg_command &cmd):
				privmsg_line(cmd.destination, cmd.serv->get_user(cmd.serv->get_nick(), true), cmd.message, cmd.sent_time) {}

			virtual operator std::string() const override;
	};
}

#endif
