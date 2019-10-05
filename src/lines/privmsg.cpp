#include "pingpong/core/util.h"
#include "spjalla/core/util.h"
#include "spjalla/lines/privmsg.h"
#include "lib/formicine/futil.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
	const std::string &message_, long stamp_, bool direct_only_):
		line(stamp_), pingpong::local(where_), speaker(speaker_), name(speaker_->name),
		self(speaker_->serv->get_nick()), message(message_), direct_only(direct_only_) {

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

	std::string privmsg_line::process(const std::string &str, bool with_time) const {
		std::string name_fmt = is_action() || is_self? ansi::bold(name) : name;
		const std::string time = with_time? lines::render_time(stamp) : "";

		if (util::is_highlight(message, self, direct_only))
			name_fmt = ansi::yellow(name_fmt);

		if (is_channel())
			name_fmt.insert(0, hat_str());

		if (is_action())
			return time + "* "_b + name_fmt + " " + pingpong::util::irc2ansi(trimmed(str));

		return time + "<"_d + name_fmt + "> "_d + pingpong::util::irc2ansi(str);
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

	privmsg_line::operator std::string() const {
		return processed;
	}

	notification_type privmsg_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only))
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string privmsg_line::to_string(const pingpong::privmsg_event &ev, bool with_time) {
		privmsg_line line {ev};
		return ansi::strip(line.process(ev.content, with_time));
	}
}
