#ifndef SPJALLA_LINES_QUIT_H_
#define SPJALLA_LINES_QUIT_H_

#include "pingpong/commands/Quit.h"
#include "pingpong/core/Defs.h"
#include "pingpong/core/User.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct QuitLine: public Line {
		std::shared_ptr<PingPong::User> user;
		std::string name;
		const std::string message;

		QuitLine(Client *parent_, std::shared_ptr<PingPong::User> user_, const std::string &message_, long stamp_):
			Line(parent_, stamp_), user(user_), name(user_->name), message(message_) {}

		QuitLine(Client *parent_, const PingPong::QuitCommand &cmd):
			QuitLine(parent_, cmd.server->getUser(cmd.server->getNick(), true, true), cmd.reason, cmd.sentTime) {}

		QuitLine(Client *parent_, const std::string &name_, const std::string &message_, long stamp_):
			Line(parent_, stamp_), user(nullptr), name(name_), message(message_) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
