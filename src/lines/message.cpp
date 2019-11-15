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

		if (!parent->cache.appearance_allow_empty_hats)
			return hats == pingpong::hat::none && !is_action()? " " : std::string(hats);

		return std::string(hats);
	}

	int message_line::get_continuation() {
#ifdef RERENDER_LINES
		render(nullptr);
#else
		if (rendered.empty()) {
			DBG("message_line::get_continuation(): rendered is empty");
			return 0;
		}
#endif
		return line::get_continuation() + parent->get_ui().render[get_format_key()].positions.at("message");
	}

	int message_line::get_name_index() {
#ifdef RERENDER_LINES
		render(nullptr);
#else
		if (rendered.empty()) {
			DBG("message_line::get_name_index(): rendered is empty");
			return 0;
		}
#endif
		return line::get_continuation() + parent->get_ui().render[get_format_key()].positions.at("nick");
	}

	notification_type message_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string message_line::render(ui::window *) {
		return parent->get_ui().render(get_format_key(), {
			{"raw_nick", name}, {"hats", hat_str()},
			{"raw_message", pingpong::util::irc2ansi(is_action()? trimmed(message) : message)}
		});
	}

	void message_line::on_mouse(const haunted::mouse_report &report) {
		//*
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
				DBG("base_continuation[" << base_continuation << "], n[" << n << "]");
				// I'm assuming there's nothing after the message in the format strings.
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
