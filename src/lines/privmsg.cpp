#include "pingpong/core/util.h"
#include "spjalla/core/util.h"
#include "spjalla/lines/privmsg.h"
#include "lib/formicine/futil.h"

namespace spjalla::lines {
	privmsg_line::privmsg_line(std::shared_ptr<pingpong::user> speaker_, const std::string &where_,
	const std::string &message_, long stamp_, bool direct_only_):
		line(stamp_), pingpong::local(where_), speaker(speaker_), name(speaker_->name),
		self(speaker_->serv->get_nick()), message(message_), verb(get_verb(message_)), body(get_body(message_)),
		direct_only(direct_only_) {

		is_self = speaker_->is_self();

		if (is_channel()) {
			std::shared_ptr<pingpong::channel> chan = get_channel(speaker->serv);
			hats = chan->get_hats(speaker);
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

	std::string privmsg_line::get_verb(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' '), length = str.length();
		return space == std::string::npos? str.substr(1, length - 2) :str.substr(space, length - space - 1);
	}

	std::string privmsg_line::get_body(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' ');
		return space == std::string::npos? "" : str.substr(space + 1, str.length() - space - 2);
	}

	bool privmsg_line::is_action(const std::string &str) {
		return !str.empty() && str.find("\1ACTION ") == 0 && str.back() == '\1';
	}

	bool privmsg_line::is_ctcp(const std::string &str) {
		return !str.empty() && str.front() == '\1' && str.back() == '\1';
	}

	bool privmsg_line::is_action() const {
		return is_action(message);
	}

	bool privmsg_line::is_ctcp() const {
		return is_ctcp(message);
	}

	std::string privmsg_line::trimmed(const std::string &str) const {
		if (str.empty() || str.front() != '\1')
			return str;

		std::string str_copy {str};
		if (str_copy.back() == '\1')
			str_copy.pop_back();

		const size_t length = str_copy.length();
		size_t i;
		for (i = 0; i < length && str_copy.at(i) != ' '; ++i);

		if (is_action())
			return str_copy.substr(i + 1);

		return str_copy;
	}

	std::string privmsg_line::hat_str() const {
		if (!is_channel())
			return "";

		return hats == pingpong::hat::none? " " : std::string(hats);
	}

	privmsg_line::operator std::string() const {
		return processed;
	}

	notification_type privmsg_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string privmsg_line::to_string(const pingpong::privmsg_event &ev, bool with_time) {
		privmsg_line line {ev};
		return ansi::strip(line.process(ev.content, with_time));
	}
}
