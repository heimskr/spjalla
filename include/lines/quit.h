#ifndef SPJALLA_LINES_QUIT_H_
#define SPJALLA_LINES_QUIT_H_

#include "pingpong/commands/quit.h"
#include "pingpong/core/ppdefs.h"
#include "pingpong/core/user.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct quit_line: haunted::ui::textline {
		std::shared_ptr<pingpong::user> user;
		const std::string message;
		long stamp;

		quit_line(std::shared_ptr<pingpong::user> user_, const std::string &message_, long stamp_):
			haunted::ui::textline(0), user(user_), message(message_), stamp(stamp_) {}

		quit_line(const pingpong::quit_command &cmd):
			quit_line(cmd.serv->get_user(cmd.serv->get_nick(), true), cmd.reason, cmd.sent_time) {}

		virtual operator std::string() const override;
	};
}

#endif
