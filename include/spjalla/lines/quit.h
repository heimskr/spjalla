#ifndef SPJALLA_LINES_QUIT_H_
#define SPJALLA_LINES_QUIT_H_

#include "pingpong/commands/quit.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/user.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct quit_line: public line {
		std::shared_ptr<pingpong::user> user;
		std::string name;
		const std::string message;

		quit_line(client *parent_, std::shared_ptr<pingpong::user> user_, const std::string &message_, long stamp_):
			line(parent_, stamp_), user(user_), name(user_->name), message(message_) {}

		quit_line(client *parent_, const pingpong::quit_command &cmd):
			quit_line(parent_, cmd.serv->get_user(cmd.serv->get_nick(), true, true), cmd.reason, cmd.sent_time) {}

		quit_line(client *parent_, const std::string &name_, const std::string &message_, long stamp_):
			line(parent_, stamp_), user(nullptr), name(name_), message(message_) {}

		virtual std::string render(ui::window *) override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
