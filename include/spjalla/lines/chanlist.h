#ifndef SPJALLA_LINES_CHANLIST_H_
#define SPJALLA_LINES_CHANLIST_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct chanlist_line: public line {
		pingpong::server * get_associated_server() const override { return chan->serv; }

		std::shared_ptr<pingpong::user> user;
		std::shared_ptr<pingpong::channel> chan;

		chanlist_line(client *parent_, std::shared_ptr<pingpong::user> user_, std::shared_ptr<pingpong::channel> chan_):
			line(parent_, 2), user(user_), chan(chan_) {}

		virtual std::string render(ui::window *) override;
	};
}

#endif
