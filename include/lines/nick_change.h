#ifndef SPJALLA_LINES_NICK_CHANGE_H_
#define SPJALLA_LINES_NICK_CHANGE_H_

#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/messages/nick.h"

#include "pingpong/events/nick.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct nick_change_line: public haunted::ui::textline {
		pingpong::user_ptr user;
		const std::string old_name;
		const std::string new_name;
		const long stamp;

		nick_change_line(pingpong::user_ptr user_, const std::string &old_name_, const std::string &new_name_,
			long stamp_): haunted::ui::textline(0), user(user_), old_name(old_name_), new_name(new_name_),
			stamp(stamp_) {}

		nick_change_line(const pingpong::nick_event &ev): nick_change_line(ev.who, ev.content, ev.who->name, ev.stamp) {
		}

		virtual operator std::string() const override;
	};
}

#endif
