#ifndef SPJALLA_LINES_USERLIST_H_
#define SPJALLA_LINES_USERLIST_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct userlist_line: haunted::ui::textline {
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user> user;
		pingpong::hat hat;

		userlist_line(std::shared_ptr<pingpong::channel> chan_, std::shared_ptr<pingpong::user> user_,
		pingpong::hat hat_ = pingpong::hat::none):
			haunted::ui::textline(0), chan(chan_), user(user_), hat(hat_) {}

		virtual operator std::string() const override;
	};
}

#endif
