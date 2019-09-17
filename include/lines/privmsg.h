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
			/** Throws an exception if chan and user are both null or both non-null. */
			void check_pointers();

			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Removes the CTCP verb from the message. */
			std::string trimmed_message() const;

		public:
			std::shared_ptr<pingpong::user> speaker;

			// Messages can be sent to users or to channels. One of these will be valid, the other null.

			std::shared_ptr<pingpong::channel> chan;
			std::shared_ptr<pingpong::user> whom;

			// We need to store a copy of the speaker's name at the time the privmsg was sent—otherwise, if they were to
			// change their name later, it would cause this line to render with the new name!
			const std::string name;
			const std::string message;
			const long stamp;

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::channel> chan_,
				std::shared_ptr<pingpong::user> whom_, const std::string &message_, long stamp_);

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::channel> chan_,
			const std::string &message_, long stamp_):
				privmsg_line(speaker_, chan_, nullptr, message_, stamp_) {}

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::user> whom_,
			const std::string &message_, long stamp_):
				privmsg_line(speaker_, nullptr, whom_, message_, stamp_) {}

			privmsg_line(const pingpong::privmsg_command &cmd):
				privmsg_line(cmd.serv->get_self(), cmd.get_channel(), cmd.get_user(), cmd.message, cmd.sent_time) {
				check_pointers();
			}

			privmsg_line(const pingpong::privmsg_event &ev):
				privmsg_line(ev.speaker, ev.chan, ev.whom, ev.message, ev.stamp) { check_pointers(); }

			virtual operator std::string() const override;
	};
}

#endif
