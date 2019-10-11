#ifndef SPJALLA_LINES_NICK_CHANGE_H_
#define SPJALLA_LINES_NICK_CHANGE_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"
#include "pingpong/core/channel.h"

#include "pingpong/messages/nick.h"

#include "pingpong/events/nick.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct nick_change_line: public line {
		std::shared_ptr<pingpong::user> user;
		const std::string old_name;
		const std::string new_name;

		nick_change_line(client *parent_, std::shared_ptr<pingpong::user> user_, const std::string &old_name_,
			const std::string &new_name_, long stamp_):
			line(parent_, stamp_), user(user_), old_name(old_name_), new_name(new_name_) {}

		nick_change_line(client *parent_, const pingpong::nick_event &ev):
			nick_change_line(parent_, ev.who, ev.content, ev.who->name, ev.stamp) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
