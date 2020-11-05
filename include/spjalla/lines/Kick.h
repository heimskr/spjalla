#ifndef SPJALLA_LINES_KICK_H_
#define SPJALLA_LINES_KICK_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/User.h"
#include "pingpong/core/Channel.h"

#include "pingpong/events/Kick.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct KickLine: public Line {
		std::shared_ptr<PingPong::Channel> channel;
		const std::string who, whom, reason;
		bool isSelf;

		KickLine(Client *parent_, std::shared_ptr<PingPong::Channel> chan, const std::string &who_,
		const std::string &whom_, const std::string &reason_, long stamp_, bool isSelf_ = false):
			Line(parent_, stamp_), channel(chan), who(who_), whom(whom_), reason(reason_), isSelf(isSelf_) {}

		KickLine(Client *parent_, std::shared_ptr<PingPong::Channel> chan, std::shared_ptr<PingPong::User> who_,
		std::shared_ptr<PingPong::User> whom_, const std::string &reason_, long stamp_):
			KickLine(parent_, chan, who_->name, whom_->name, reason_, stamp_, whom_->isSelf()) {}

		KickLine(Client *parent_, const PingPong::KickEvent &ev):
			KickLine(parent_, ev.channel, ev.who, ev.whom, ev.content, ev.stamp) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override;
	};
}

#endif
