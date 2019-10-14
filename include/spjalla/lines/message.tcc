#include "pingpong/core/util.h"
#include "spjalla/core/util.h"
#include "spjalla/lines/message.h"
#include "lib/formicine/futil.h"

namespace spjalla::lines {
	template <typename T>
	message_line<T>::message_line(client *parent_, std::shared_ptr<pingpong::user> speaker, const std::string &where_,
	const std::string &message_, long stamp_, bool direct_only_):
		line(parent_, stamp_), pingpong::local(where_), name(speaker->name), self(speaker->serv->get_nick()),
		message(message_), verb(get_verb(message_)), body(get_body(message_)), direct_only(direct_only_) {

		is_self = speaker->is_self();

		if (is_channel() && speaker) {
			std::shared_ptr<pingpong::channel> chan = get_channel(speaker->serv);
			hats = chan->get_hats(speaker);
		}

		continuation = get_continuation() + lines::time_length;
		processed = process(message_);
	}

	template <typename T>
	message_line<T>::message_line(client *parent_, const std::string &name_, const std::string &where_,
	const std::string &self_, const std::string &message_, long stamp_, const pingpong::hat_set &hats_,
	bool direct_only_):
	line(parent_, stamp_), pingpong::local(where_), name(name_), self(self_), message(message_),
	verb(get_verb(message_)), body(get_body(message_)), hats(hats_), direct_only(direct_only_) {
		is_self = name_ == self_;
		continuation = get_continuation() + lines::time_length;
		processed = process(message_);
	}

	template <typename T>
	message_line<T>::message_line(client *parent_, const std::string &combined_, const std::string &where_,
	const std::string &self_, const std::string &message_, long stamp_, bool direct_only_):
	line(parent_, stamp_), pingpong::local(where_), self(self_), message(message_), verb(get_verb(message_)),
	body(get_body(message_)), direct_only(direct_only_) {
		std::tie(hats, name) = pingpong::hat_set::separate(combined_);
		is_self = name == self_;
		continuation = get_continuation() + lines::time_length;
		processed = process(message_);
	}

	template <typename T>
	size_t message_line<T>::get_continuation() const {
		std::string format = ansi::strip(is_action()? T::action : T::message);

		const size_t mpos = format.find("%m");
		if (mpos == std::string::npos)
			throw std::invalid_argument("Invalid message format string");

		// If the speaker comes before the message in the format, we need to adjust the return value accordingly.
		return mpos + (format.find("%s") < mpos? name.length() - 2 : 0);
	}

	template <typename T>
	std::string message_line<T>::process(const std::string &str, bool with_time) const {
		std::string name_fmt = is_action() || is_self? ansi::bold(name) : name;
		const std::string time = with_time? lines::render_time(stamp) : "";

		if (util::is_highlight(message, self, direct_only))
			name_fmt = ansi::yellow(name_fmt);

		std::string out = ansi::format(is_action()? T::action : T::message);
		const size_t spos = out.find("%s");
		if (spos == std::string::npos)
			throw std::invalid_argument("Invalid message format string");

		out.erase(spos, 2);
		out.insert(spos, name_fmt);

		const size_t hpos = out.find("%h");
		if (hpos != std::string::npos) {
			out.erase(hpos, 2);
			if (is_channel())
				out.insert(hpos, hat_str());
		}

		const size_t mpos = out.find("%m");
		if (mpos == std::string::npos)
			throw std::invalid_argument("Invalid message format string");

		out.erase(mpos, 2);
		out.insert(mpos, pingpong::util::irc2ansi(is_action()? trimmed(str) : str));

		T::postprocess(this, out);
		return time + out;
	}

	template <typename T>
	std::string message_line<T>::get_verb(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' '), length = str.length();
		return space == std::string::npos? str.substr(1, length - 2) :str.substr(space, length - space - 1);
	}

	template <typename T>
	std::string message_line<T>::get_body(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' ');
		return space == std::string::npos? "" : str.substr(space + 1, str.length() - space - 2);
	}

	template <typename T>
	bool message_line<T>::is_action(const std::string &str) {
		return !str.empty() && str.find("\1ACTION ") == 0 && str.back() == '\1';
	}

	template <typename T>
	bool message_line<T>::is_ctcp(const std::string &str) {
		return !str.empty() && str.front() == '\1' && str.back() == '\1';
	}

	template <typename T>
	bool message_line<T>::is_action() const {
		return is_action(message);
	}

	template <typename T>
	bool message_line<T>::is_ctcp() const {
		return is_ctcp(message);
	}

	template <typename T>
	std::string message_line<T>::trimmed(const std::string &str) const {
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

	template <typename T>
	std::string message_line<T>::hat_str() const {
		if (!is_channel())
			return "";

		return hats == pingpong::hat::none? " " : std::string(hats);
	}

	template <typename T>
	message_line<T>::operator std::string() const {
		return processed;
	}

	template <typename T>
	notification_type message_line<T>::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}
}
