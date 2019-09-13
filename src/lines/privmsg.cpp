#include "lines/privmsg.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(pingpong::channel_ptr chan_, pingpong::user_ptr user_, const std::string &message_,
	long stamp_): haunted::ui::textline(0), chan(chan_), user(user_), message(message_), stamp(stamp_) {
		if (is_action()) {
			continuation = ("[xx:xx:xx] <." + user_->name + "> ").length();
		} else {
			continuation = ("[xx:xx:xx] * " + user_->name + " ").length();
		}
	}

	bool privmsg_line::is_action() const {
		return !message.empty() && message.find("\1ACTION ") == 0 && message.back() == '\1';
	}

	std::string privmsg_line::trimmed_message() const {
		if (message.empty() || message.front() != '\1')
			return message;

		std::string message_copy {message};
		if (message_copy.back() == '\1')
			message_copy.pop_back();

		
		size_t i, length = message_copy.length();
		for (i = 0; i < length && message_copy.at(i) != ' '; ++i);
		return message_copy.at(i) == ' '? message_copy.substr(i + 1) : message_copy;
		
	}

	privmsg_line::operator std::string() const {
		pingpong::hat hat = pingpong::hat::none;
		if (chan->hats.count(user) != 0)
			hat = chan->hats.at(user);

		const std::string hat_str(1, static_cast<char>(hat));

		return is_action()? (lines::render_time(stamp) + "* "_b + ansi::bold(user->name) + " " + trimmed_message())
		                  : (lines::render_time(stamp) + "<"_d + hat_str + user->name + "> "_d + message);
	}
}
