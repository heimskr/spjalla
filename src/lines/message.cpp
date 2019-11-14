#include "spjalla/lines/message.h"

namespace spjalla::lines {
	message_line::message_line(client *parent_, std::shared_ptr<pingpong::user> speaker, const std::string &where_,
	                           const std::string &message_, long stamp_, bool direct_only_):
	line(parent_, stamp_), pingpong::local(where_), name(speaker->name), self(speaker->serv->get_nick()),
	message(message_), verb(get_verb(message_)), body(get_body(message_)), direct_only(direct_only_),
	serv(speaker->serv) {

		is_self = speaker->is_self();

		if (is_channel() && speaker) {
			std::shared_ptr<pingpong::channel> chan = get_channel(speaker->serv);
			hats = chan->get_hats(speaker);
		}
	}

	message_line::message_line(client *parent_, const std::string &name_, const std::string &where_,
	                           const std::string &self_, const std::string &message_, long stamp_,
	                           const pingpong::hat_set &hats_, bool direct_only_):
	line(parent_, stamp_), pingpong::local(where_), name(name_), self(self_), message(message_),
	verb(get_verb(message_)), body(get_body(message_)), hats(hats_), direct_only(direct_only_) {
		is_self = name_ == self_;
	}

	message_line::message_line(client *parent_, const std::string &combined_, const std::string &where_,
	                           const std::string &self_, const std::string &message_, long stamp_, bool direct_only_):
	line(parent_, stamp_), pingpong::local(where_), self(self_), message(message_), verb(get_verb(message_)),
	body(get_body(message_)), direct_only(direct_only_) {
		std::tie(hats, name) = pingpong::hat_set::separate(combined_);
		is_self = name == self_;
	}


// Private instance methods


	std::string message_line::get_verb(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' '), length = str.length();
		return space == std::string::npos? str.substr(1, length - 2) : str.substr(space, length - space - 1);
	}

	std::string message_line::get_body(const std::string &str) {
		if (!is_ctcp(str))
			return "";
		const size_t space = str.find(' ');
		return space == std::string::npos? "" : str.substr(space + 1, str.length() - space - 2);
	}

	bool message_line::is_action(const std::string &str) {
		return !str.empty() && str.find("\1ACTION ") == 0 && str.back() == '\1';
	}

	bool message_line::is_ctcp(const std::string &str) {
		return !str.empty() && str.front() == '\1' && str.back() == '\1';
	}


// Protected instance methods


	// std::string message_line::process(const std::string &) {
		// std::string name_fmt = is_action() || is_self? ansi::bold(render_name()) : render_name();
		// std::string out = ansi::format(get_format());

		// if (util::is_highlight(message, self, direct_only))
		// 	name_fmt = ansi::yellow(name_fmt);

		/*
		const size_t spos = out.find("#s");
		if (spos == std::string::npos)
			throw std::invalid_argument("Invalid message format string");

		out.erase(spos, 2);
		out.insert(spos, name_fmt);

		const size_t hpos = out.find("#h");
		if (hpos != std::string::npos) {
			out.erase(hpos, 2);
			if (is_channel())
				out.insert(hpos, hat_str());
		}

		const size_t mpos = out.find("#m");
		if (mpos == std::string::npos)
			throw std::invalid_argument("Invalid message format string");

		out.erase(mpos, 2);
		processed_message = pingpong::util::irc2ansi(is_action()? trimmed(str) : str);
		out.insert(mpos, processed_message);

		T::postprocess(this, out);
		return out;
		//*/
	// 	return "???";
	// }


// Public instance methods


	bool message_line::is_action() const {
		return is_action(message);
	}

	bool message_line::is_ctcp() const {
		return is_ctcp(message);
	}

	std::string message_line::trimmed(const std::string &str) const {
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

	std::string message_line::hat_str() const {
		if (!is_channel())
			return "";

		return hats == pingpong::hat::none? " " : std::string(hats);
	}

	notification_type message_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	void message_line::on_mouse(const haunted::mouse_report &) {
		/*
		if (report.action == haunted::mouse_action::up) {
			const int name_index = get_name_index();
			if (name_index <= report.x && report.x < name_index + static_cast<int>(name.length())) {
				if (!serv) {
					DBG("Can't query: server is null");
				} else {
					parent->get_ui().focus_window(parent->query(name, serv));
				}

				return;
			}

			if (box) {
				// Compute the clicked character's index within the message.
				ssize_t n = -get_continuation();
				// I'm there's nothing after the message in the format strings.
				ssize_t message_width = box->get_position().width + n;
				n += report.x + report.y * message_width;
				ssize_t windex, sindex;
				std::tie(windex, sindex) = formicine::util::word_indices(message, n);
				std::string word = formicine::util::nth_word(message, windex);
				DBG("w[" << windex << "], s[" << sindex << "], word[" << word << "]");
				if (!word.empty() && word.front() == '#')
					pingpong::join_command(serv, word).send();
			}
		}
		//*/
	}
}
