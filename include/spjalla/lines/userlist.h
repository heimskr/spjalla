#ifndef SPJALLA_LINES_USERLIST_H_
#define SPJALLA_LINES_USERLIST_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/Channel.h"
#include "pingpong/core/User.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct UserlistLine: public Line {
		std::shared_ptr<PingPong::Channel> channel;
		std::shared_ptr<PingPong::User> user;
		size_t pad;

		UserlistLine(Client *parent_, std::shared_ptr<PingPong::Channel> chan, std::shared_ptr<PingPong::User> user_,
		size_t pad_ = 0):
			Line(parent_, 2), channel(chan), user(user_), pad(pad_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
