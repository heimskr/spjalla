#ifndef SPJALLA_LINES_MESSAGE_H_
#define SPJALLA_LINES_MESSAGE_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	/**
	 * Base class for lines representing PRIVMSGs and related messages like NOTICE.
	 */
	template <typename T>
	class message_line: public line, public pingpong::local {
		 /**
		  *   struct T { static std::string message, action; };
		  * 
		  * `message` and `action` are strings in which "%s" is replaced with the name of the speaker and "%m" is
		  * replaced with the message. `action` is used for CTCP ACTIONs and `message` is used for everything else.
		  */

		private:
			bool is_self = false;

			/** Finds the continuation for the line. */
			size_t get_continuation() const;
			
			/** If the given message is a CTCP message, this function returns its verb. Otherwise, it returns an empty
			 *  string. */
			static std::string get_verb(const std::string &);

			/** If the given message is a CTCP message, this function returns its body. Otherwise, it returns an empty
			 *  string. */
			static std::string get_body(const std::string &);

			/** Returns whether a message is an action (CTCP ACTION). */
			static bool is_action(const std::string &);

			/** Returns whether a message is a CTCP message. */
			static bool is_ctcp(const std::string &);

		protected:
			/** Formats a message by processing colors and actions and adding the user's name. */
			std::string process(const std::string &, bool with_time = true) const;

			/** Performs the last step of processing. */
			virtual void postprocess(std::string &) const;

		public:
			// We need to store a copy of the speaker's name at the time the message was sent—otherwise, if they were to
			// change their name later, it would cause this line to render with the new name!
			std::string name, self, message, verb, body;

			// It's also necessary to store the user's hat(s) at the time the message was sent (provided the message was
			// to a channel and not directly to you).
			pingpong::hat_set hats {};

			const bool direct_only;

			/** The message after colors and actions have been processed. */
			std::string processed;

			message_line(client *parent_, std::shared_ptr<pingpong::user> speaker, const std::string &where_,
			const std::string &message_, long stamp_, bool direct_only_ = false);

			message_line(client *parent_, std::shared_ptr<pingpong::user> speaker,
			std::shared_ptr<pingpong::channel> chan_, const std::string &message_, long stamp_,
			bool direct_only_ = false):
				message_line(parent_, speaker, chan_->name, message_, stamp_, direct_only_) {}

			message_line(client *parent_, std::shared_ptr<pingpong::user> speaker,
			std::shared_ptr<pingpong::user> whom_, const std::string &message_, long stamp_, bool direct_only_ = false):
				message_line(parent_, speaker, whom_->name, message_, stamp_, direct_only_) {}

			message_line(client *, const std::string &name_, const std::string &where_, const std::string &self_,
			const std::string &message_, long, const pingpong::hat_set &, bool direct_only_ = false);

			message_line(client *, const std::string &combined_, const std::string &where_, const std::string &self_,
			const std::string &message_, long, bool direct_only_ = false);

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
	};
}

#include "message.tcc"

#endif
