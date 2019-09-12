#include "lines/lines.h"
#include "lines/privmsg.h"

#include "formicine/ansi.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_,
	long stamp_): haunted::ui::textline(0), chan(chan_), user(user_), message(message_), stamp(stamp_) {
		const std::string start = "[xx:xx:xx] <" + user_->name + "> ";
		continuation = start.length();
	}

	privmsg_line::operator std::string() const {
		pingpong::hat hat = pingpong::hat::none;

		if (chan->hats.count(user) != 0)
			hat = chan->hats.at(user);

		return lines::render_time(stamp) + "<"_d + std::string(1, static_cast<char>(hat)) + user->name + "> "_d
			+ message;
	}
}
