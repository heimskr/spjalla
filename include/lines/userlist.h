#ifndef SPJALLA_LINES_USERLIST_H_
#define SPJALLA_LINES_USERLIST_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct userlist_line: haunted::ui::textline {
		pingpong::channel_ptr chan;
		pingpong::user_ptr user;
		pingpong::hat hat;

		userlist_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, pingpong::hat hat_ = pingpong::hat::none):
			haunted::ui::textline(0), chan(chan_), user(user_), hat(hat_) {}

		virtual operator std::string() const override;
	};
}

#endif
