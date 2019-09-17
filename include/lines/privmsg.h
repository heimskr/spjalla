#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/commands/privmsg.h"

#include "pingpong/events/privmsg.h"

#include "lines/lines.h"

namespace spjalla::lines {
	class privmsg_line: public haunted::ui::textline {
		private:
			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Removes the CTCP verb from the message. */
			std::string trimmed_message() const;

		public:
			std::shared_ptr<pingpong::channel> chan;
			std::shared_ptr<pingpong::user> user;
			const std::string name;
			const std::string message;
			const long stamp;

			privmsg_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_,
				const std::string &message_, long stamp_);

			privmsg_line(const pingpong::privmsg_command &cmd):
				privmsg_line(cmd.destination, cmd.serv->get_user(cmd.serv->get_nick(), true), cmd.message,
				cmd.sent_time) {}

			privmsg_line(const pingpong::privmsg_event &ev): privmsg_line(ev.chan, ev.who, ev.content, ev.stamp) {}

			virtual operator std::string() const override;
	};
}

#endif
