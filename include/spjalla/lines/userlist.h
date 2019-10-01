#ifndef SPJALLA_LINES_USERLIST_H_
#define SPJALLA_LINES_USERLIST_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct userlist_line: line {
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user> user;

		userlist_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_):
			line(2), chan(chan_), user(user_) {}

		virtual operator std::string() const override;
	};
}

#endif
