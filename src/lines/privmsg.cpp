#include "spjalla/lines/privmsg.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
	const std::string &message_, long stamp_):
		haunted::ui::textline(0), pingpong::local(where_), speaker(speaker_), name(speaker_->name), message(message_),
		stamp(stamp_) {

		is_self = speaker_->is_self();

		if (is_channel()) {
			std::shared_ptr<pingpong::channel> chan = get_channel(speaker->serv);
			if (chan->hats.count(speaker) != 0)
				hat = chan->hats.at(speaker);
		}

		if (is_action())
			continuation = ("[xx:xx:xx] * " + name + " ").length();
		else
			continuation = ("[xx:xx:xx] <." + name + "> ").length() - (is_user()? 1 : 0);

		processed = process(message_);
	}

	bool privmsg_line::is_action() const {
		return !message.empty() && message.find("\1ACTION ") == 0 && message.back() == '\1';
	}

	std::string privmsg_line::trimmed(const std::string &str) const {
		if (str.empty() || str.front() != '\1')
			return str;

		std::string str_copy {str};
		if (str_copy.back() == '\1')
			str_copy.pop_back();

		size_t i, length = str_copy.length();
		for (i = 0; i < length && str_copy.at(i) != ' '; ++i);
		return str_copy.at(i) == ' '? str_copy.substr(i + 1) : str_copy;
	}

	std::string privmsg_line::hat_str() const {
		return is_channel()? std::string(1, static_cast<char>(hat)) : "";
	}

	std::string privmsg_line::process(const std::string &str) const {
		std::string name_fmt = is_action() || is_self? ansi::bold(name) : name;
		if (is_channel())
			name_fmt.insert(0, hat_str());

		if (is_action())
			return lines::render_time(stamp) + "* "_b + name_fmt + " " + pingpong::util::irc2ansi(trimmed(str));

		return lines::render_time(stamp) + "<"_d + name_fmt + "> "_d + pingpong::util::irc2ansi(str);
	}

	privmsg_line::operator std::string() const {
		return processed;
	}
}
