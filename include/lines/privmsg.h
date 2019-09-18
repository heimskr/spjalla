#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/privmsg.h"
#include "pingpong/events/privmsg.h"

#include "lines/lines.h"

namespace spjalla::lines {
	class privmsg_line: public haunted::ui::textline, public pingpong::local {
		private:
			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Removes the CTCP verb from the message. */
			std::string trimmed_message() const;

			/** Returns a string representing the user's hat (empty if the destination isn't a channel). */
			std::string hat_str() const;

		public:
			std::shared_ptr<pingpong::user> speaker;

			// We need to store a copy of the speaker's name at the time the privmsg was sent—otherwise, if they were to
			// change their name later, it would cause this line to render with the new name!
			const std::string name;
			// It's also necessary to store the user's hat at the time the message was sent (provided the message was to
			// a channel and not directly to you).
			pingpong::hat hat = pingpong::hat::none;
			const std::string message;
			const long stamp;

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
			const std::string &message_, long stamp_);

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::channel> chan_,
			const std::string &message_, long stamp_):
				privmsg_line(speaker_, chan_->name, message_, stamp_) {}

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::user> whom_,
			const std::string &message_, long stamp_):
				privmsg_line(speaker_, whom_->name, message_, stamp_) {}

			privmsg_line(const pingpong::privmsg_command &cmd):
				privmsg_line(cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time) {}

			privmsg_line(const pingpong::privmsg_event &ev):
				privmsg_line(ev.speaker, ev.where, ev.content, ev.stamp) {}

			virtual operator std::string() const override;
	};
}

#endif
