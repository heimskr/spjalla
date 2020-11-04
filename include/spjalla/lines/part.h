#ifndef SPJALLA_LINES_part_H_
#define SPJALLA_LINES_part_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/User.h"
#include "pingpong/core/Channel.h"

#include "pingpong/events/Part.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct PartLine: public Line {
		std::shared_ptr<PingPong::Channel> channel;
		std::shared_ptr<PingPong::User> user;
		const std::string channelName, name, reason;

		PartLine(Client *parent_, std::shared_ptr<PingPong::Channel> chan, std::shared_ptr<PingPong::User> user_,
		const std::string &reason_, long stamp_):
			Line(parent_, stamp_), channel(chan), user(user_), channelName(chan->name), name(user_->name),
			reason(reason_) {}

		PartLine(Client *parent_, const std::string &channel_name, const std::string &name_, const std::string &reason_,
		long stamp_):
			Line(parent_, stamp_), channel(nullptr), user(nullptr), channelName(channel_name), name(name_),
			reason(reason_) {}

		PartLine(Client *parent_, const PingPong::PartEvent &ev):
		PartLine(parent_, ev.channel, ev.who, ev.content, ev.stamp) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
