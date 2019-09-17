#include "lines/privmsg.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(std::shared_ptr<pingpong::user> speaker_, std::shared_ptr<pingpong::channel> chan_,
	std::shared_ptr<pingpong::user> whom_, const std::string &message_, long stamp_):
		haunted::ui::textline(0), speaker(speaker_), chan(chan_), whom(whom_), name(speaker_->name), message(message_),
		stamp(stamp_) {

		check_pointers();

		if (is_action()) {
			continuation = ("[xx:xx:xx] * " + name + " ").length();
		} else {
			continuation = ("[xx:xx:xx] <." + name + "> ").length() - (whom? 1 : 0);
		}
	}

	void privmsg_line::check_pointers() {
		DBG("Checking pointers.");
		if (whom && chan)
			throw std::runtime_error("Both user and channel destinations are non-null in privmsg_line.");
		else if (!whom && !chan)
			throw std::runtime_error("Both user and channel destinations are null in privmsg_line.");
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
		if (chan) {
			pingpong::hat hat = pingpong::hat::none;
			if (chan->hats.count(whom) != 0)
				hat = chan->hats.at(whom);

			const std::string hat_str(1, static_cast<char>(hat));

			return is_action()? (lines::render_time(stamp) + "* "_b + ansi::bold(name) + " " + trimmed_message())
							: (lines::render_time(stamp) + "<"_d + hat_str + name + "> "_d + message);
		} else {
			return is_action()? (lines::render_time(stamp) + "* "_b + ansi::bold(name) + " " + trimmed_message())
							: (lines::render_time(stamp) + "<"_d + name + "> "_d + message);
		}
	}
}
