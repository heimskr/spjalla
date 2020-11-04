#ifndef SPJALLA_LINES_CHANLIST_H_
#define SPJALLA_LINES_CHANLIST_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/Channel.h"
#include "pingpong/core/User.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct ChanlistLine: public Line {
		std::shared_ptr<PingPong::User> user;
		std::shared_ptr<PingPong::Channel> channel;

		ChanlistLine(Client *parent_, std::shared_ptr<PingPong::User> user_, std::shared_ptr<PingPong::Channel> chan_):
			Line(parent_, 2), user(user_), channel(chan_) {}

		PingPong::Server * getAssociatedServer() const override { return channel->server; }
		virtual std::string render(UI::Window *) override;
	};
}

#endif
