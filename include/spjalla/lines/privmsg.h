#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/privmsg.h"
#include "pingpong/events/privmsg.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class privmsg_line: public line, public pingpong::local {
		private:
			bool is_self = false;

			/** Formats a message by processing colors and actions and adding the user's name. */
			std::string process(const std::string &, bool with_time = true) const;

			/** If the given message is a CTCP message, this function returns its verb. Otherwise, it returns an empty
			 *  string. */
			static std::string get_verb(const std::string &);

			/** If the given message is a CTCP message, this function returns its body. Otherwise, it returns an empty
			 *  string. */
			static std::string get_body(const std::string &);

			/** Returns whether the message is an action (CTCP ACTION). */
			static bool is_action(const std::string &);

			/** Returns whether the message is a CTCP message. */
			static bool is_ctcp(const std::string &);

		public:
			std::shared_ptr<pingpong::user> speaker;

			// We need to store a copy of the speaker's name at the time the privmsg was sent—otherwise, if they were to
			// change their name later, it would cause this line to render with the new name!
			const std::string name, self, message, verb, body;

			// It's also necessary to store the user's hat(s) at the time the message was sent (provided the message was
			// to a channel and not directly to you).
			pingpong::hat_set hats {};

			const bool direct_only;

			/** The message after colors and actions have been processed. */
			std::string processed;

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
			const std::string &message_, long stamp_, bool direct_only_ = false);

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::channel> chan_,
			const std::string &message_, long stamp_, bool direct_only_ = false):
				privmsg_line(speaker_, chan_->name, message_, stamp_, direct_only_) {}

			privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::user> whom_,
			const std::string &message_, long stamp_, bool direct_only_ = false):
				privmsg_line(speaker_, whom_->name, message_, stamp_, direct_only_) {}

			privmsg_line(const pingpong::privmsg_command &cmd, bool direct_only_ = false):
				privmsg_line(cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time, direct_only_) {}

			privmsg_line(const pingpong::privmsg_event &ev, bool direct_only_ = false):
				privmsg_line(ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {}

			/** Returns whether the message is an action (CTCP ACTION). */
			bool is_action() const;

			/** Returns whether the message is a CTCP message. */
			bool is_ctcp() const;

			/** Removes the CTCP verb from a message. */
			std::string trimmed(const std::string &) const;

			/** Returns a string representing the user's hat (empty if the destination isn't a channel). */
			std::string hat_str() const;

			virtual operator std::string() const override;
			virtual notification_type get_notification_type() const override;

			static std::string to_string(const pingpong::privmsg_event &, bool with_time = true);
	};
}

#endif
