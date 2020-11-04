#ifndef SPJALLA_LINES_JOIN_H_
#define SPJALLA_LINES_JOIN_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/User.h"
#include "pingpong/core/Channel.h"

#include "pingpong/events/Join.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct JoinLine: public Line {
		std::shared_ptr<PingPong::Channel> channel;
		std::shared_ptr<PingPong::User> user;
		const std::string channelName;
		const std::string name;

		JoinLine(Client *parent_, std::shared_ptr<PingPong::Channel> chan, std::shared_ptr<PingPong::User> user_,
		long stamp_):
			Line(parent_, stamp_), channel(chan), user(user_), channelName(chan->name), name(user_->name) {}

		JoinLine(Client *parent_, const std::string &chan_name, const std::string &name_, long stamp_):
			Line(parent_, stamp_), channel(nullptr), user(nullptr), channelName(chan_name), name(name_) {}

		JoinLine(Client *parent_, const PingPong::JoinEvent &ev):
			JoinLine(parent_, ev.channel, ev.who, ev.stamp) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
