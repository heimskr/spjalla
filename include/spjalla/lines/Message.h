#ifndef SPJALLA_LINES_MESSAGE_H_
#define SPJALLA_LINES_MESSAGE_H_

#include "pingpong/commands/Join.h"

#include "pingpong/core/Defs.h"
#include "pingpong/core/Hats.h"
#include "pingpong/core/Local.h"
#include "pingpong/core/Util.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"

#include "spjalla/lines/Message.h"
#include "spjalla/lines/Line.h"

#include "lib/formicine/ansi.h"
#include "lib/formicine/futil.h"

namespace Spjalla::Lines {
	/**
	 * Base class for lines representing PRIVMSGs and related messages like NOTICE.
	 */
	class MessageLine: public Line, public PingPong::Local {
		private:
			bool isSelf = false;
			std::string processedMessage;
			
			/** If the given message is a CTCP message, this function returns its verb. Otherwise, it returns an empty
			 *  string. */
			static std::string getVerb(const std::string &);

			/** If the given message is a CTCP message, this function returns its body. Otherwise, it returns an empty
			 *  string. */
			static std::string getBody(const std::string &);

			/** Returns whether a message is an action (CTCP ACTION). */
			static bool isAction(const std::string &);

			/** Returns whether a message is a CTCP message. */
			static bool isCTCP(const std::string &);

		protected:
			PingPong::Server * getAssociatedServer() const override { return server; }

		public:
			// We need to store a copy of the speaker's name at the time the message was sent—otherwise, if they were to
			// change their name later, it would cause this line to render with the new name!
			std::string name, self, message, verb, body;

			// It's also necessary to store the user's hat(s) at the time the message was sent (provided the message was
			// to a channel and not directly to you).
			PingPong::HatSet hats {};

			const bool directOnly;
			PingPong::Server *server = nullptr;

			MessageLine(Client *, std::shared_ptr<PingPong::User>, const std::string &where_,
			            const std::string &message_, long stamp_, bool direct_only_ = false);

			MessageLine(Client *parent_, std::shared_ptr<PingPong::User> speaker,
			            std::shared_ptr<PingPong::Channel> chan_, const std::string &message_, long stamp_,
			            bool direct_only_ = false):
				MessageLine(parent_, speaker, chan_->name, message_, stamp_, direct_only_) {}

			MessageLine(Client *parent_, std::shared_ptr<PingPong::User> speaker,
			            std::shared_ptr<PingPong::User> whom_, const std::string &message_, long stamp_,
			            bool direct_only_ = false):
				MessageLine(parent_, speaker, whom_->name, message_, stamp_, direct_only_) {}

			MessageLine(Client *, const std::string &name_, const std::string &where_, const std::string &self_,
			            const std::string &message_, long, const PingPong::HatSet &, bool direct_only_ = false);

			MessageLine(Client *, const std::string &combined_, const std::string &where_, const std::string &self_,
			            const std::string &message_, long, bool direct_only_ = false);

			/** Returns whether the message is an action (CTCP ACTION). */
			bool isAction() const;

			/** Returns whether the message is a CTCP message. */
			bool isCTCP() const;

			/** Removes the CTCP verb from a message. */
			std::string trimmed(const std::string &) const;

			/** Returns a string representing the user's hat (empty if the destination isn't a channel). */
			std::string hatString() const;

			int getContinuation() override;
			int getNameIndex();

			virtual std::string getFormatKey() const = 0;

			// virtual std::string render(UI::Window *);
			virtual NotificationType getNotificationType() const override;
			virtual std::string render(UI::Window *) override;

			void onMouse(const Haunted::MouseReport &) override;
	};
}

#endif
