#include "lines/privmsg.h"

#include "formicine/ansi.h"

namespace spjalla::lines {	
	privmsg_line::privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_):
	haunted::ui::textline(0), chan(chan_), user(user_), message(message_) {
		const std::string start = "<" + user_->name + "> ";
		continuation = start.length();
	}

	privmsg_line::operator std::string() const {
		return "<"_d + user->name + "> "_d + message;
	}
}
