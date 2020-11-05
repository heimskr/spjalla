#ifndef SPJALLA_LINES_PRIVMSG_H_
#define SPJALLA_LINES_PRIVMSG_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/Hats.h"
#include "pingpong/core/Local.h"

#include "pingpong/commands/Privmsg.h"
#include "pingpong/events/Privmsg.h"

#include "spjalla/lines/Message.h"

namespace Spjalla::Lines {
	class PrivmsgLine: public MessageLine {
		public:
			using MessageLine::MessageLine;

			PrivmsgLine(Client *parent_, const PingPong::PrivmsgCommand &cmd, bool direct_only = false):
				PrivmsgLine(parent_, cmd.server->getSelf(), cmd.where, cmd.message, cmd.sentTime, direct_only) {}

			PrivmsgLine(Client *parent_, const PingPong::PrivmsgEvent &ev, bool direct_only = false):
				PrivmsgLine(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only) {}

			NotificationType getNotificationType() const override;
			std::string getFormatKey() const override;
	};
}

#endif
