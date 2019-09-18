#include "lines/privmsg.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
	const std::string &message_, long stamp_):
		haunted::ui::textline(0), pingpong::local(where_), speaker(speaker_), name(speaker_->name), message(message_),
		stamp(stamp_) {

		if (is_channel()) {
			std::shared_ptr<pingpong::channel> chan = get_channel(speaker->serv);
			if (chan->hats.count(speaker) != 0)
				hat = chan->hats.at(speaker);
		}

		if (is_action())
			continuation = ("[xx:xx:xx] * " + name + " ").length();
		else
			continuation = ("[xx:xx:xx] <." + name + "> ").length() - (is_user()? 1 : 0);
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

	std::string privmsg_line::hat_str() const {
		return is_channel()? std::string(1, static_cast<char>(hat)) : "";
	}

	privmsg_line::operator std::string() const {
		if (is_channel()) {
			return is_action()? (lines::render_time(stamp) + "* "_b + ansi::bold(hat_str() + name) + " " +
				trimmed_message()) : (lines::render_time(stamp) + "<"_d + hat_str() + name + "> "_d + message);
		} else {
			return is_action()? (lines::render_time(stamp) + "* "_b + ansi::bold(name) + " " + trimmed_message())
							: (lines::render_time(stamp) + "<"_d + name + "> "_d + message);
		}
	}
}
