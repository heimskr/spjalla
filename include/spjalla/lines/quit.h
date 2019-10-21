#ifndef SPJALLA_LINES_QUIT_H_
#define SPJALLA_LINES_QUIT_H_

#include "pingpong/commands/quit.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct quit_line: public line {
		std::shared_ptr<pingpong::user> user;
		const std::string message;

		quit_line(client *parent_, std::shared_ptr<pingpong::user> user_, const std::string &message_, long stamp_):
			line(parent_, stamp_), user(user_), message(message_) {}

		quit_line(client *parent_, const pingpong::quit_command &cmd):
			quit_line(parent_, cmd.serv->get_user(cmd.serv->get_nick(), true), cmd.reason, cmd.sent_time) {}

		virtual std::string render(ui::window *) override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
