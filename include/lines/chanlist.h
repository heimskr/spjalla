#ifndef SPJALLA_LINES_CHANLIST_H_
#define SPJALLA_LINES_CHANLIST_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct chanlist_line: haunted::ui::textline {
		std::shared_ptr<pingpong::user> user;
		std::shared_ptr<pingpong::channel> chan;

		chanlist_line(std::shared_ptr<pingpong::user> user_, std::shared_ptr<pingpong::channel> chan_):
			haunted::ui::textline(2), user(user_), chan(chan_) {}

		virtual operator std::string() const override;
	};
}

#endif
