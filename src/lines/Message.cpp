#include "spjalla/lines/Message.h"

namespace Spjalla::Lines {
	MessageLine::MessageLine(Client *parent_, std::shared_ptr<PingPong::User> speaker, const std::string &where_,
	                         const std::string &message_, long stamp_, bool direct_only):
	Line(parent_, stamp_), PingPong::Local(where_), name(speaker->name), self(speaker->server->getNick()),
	message(message_), verb(getVerb(message_)), body(getBody(message_)), directOnly(direct_only),
	server(speaker->server) {
		isSelf = speaker->isSelf();
		if (isChannel() && speaker)
			hats = getChannel(speaker->server)->getHats(speaker);
	}

	MessageLine::MessageLine(Client *parent_, const std::string &name_, const std::string &where_,
	                         const std::string &self_, const std::string &message_, long stamp_,
	                         const PingPong::HatSet &hats_, bool direct_only):
	Line(parent_, stamp_), PingPong::Local(where_), name(name_), self(self_), message(message_),
	verb(getVerb(message_)), body(getBody(message_)), hats(hats_), directOnly(direct_only) {
		isSelf = name_ == self_;
	}

	MessageLine::MessageLine(Client *parent_, const std::string &combined_, const std::string &where_,
	                         const std::string &self_, const std::string &message_, long stamp_, bool direct_only):
	Line(parent_, stamp_), PingPong::Local(where_), self(self_), message(message_), verb(getVerb(message_)),
	body(getBody(message_)), directOnly(direct_only) {
		std::tie(hats, name) = PingPong::HatSet::separate(combined_);
		isSelf = name == self_;
	}


// Private instance methods


	std::string MessageLine::getVerb(const std::string &str) {
		if (!isCTCP(str))
			return "";
		const size_t space = str.find(' '), length = str.length();
		return space == std::string::npos? str.substr(1, length - 2) : str.substr(space, length - space - 1);
	}

	std::string MessageLine::getBody(const std::string &str) {
		if (!isCTCP(str))
			return "";
		const size_t space = str.find(' ');
		return space == std::string::npos? "" : str.substr(space + 1, str.length() - space - 2);
	}

	bool MessageLine::isAction(const std::string &str) {
		return !str.empty() && str.find("\1ACTION ") == 0 && str.back() == '\1';
	}

	bool MessageLine::isCTCP(const std::string &str) {
		return !str.empty() && str.front() == '\1' && str.back() == '\1';
	}


// Public instance methods


	bool MessageLine::isAction() const {
		return isAction(message);
	}

	bool MessageLine::isCTCP() const {
		return isCTCP(message);
	}

	std::string MessageLine::trimmed(const std::string &str) const {
		if (str.empty() || str.front() != '\1')
			return str;

		std::string str_copy {str};
		if (str_copy.back() == '\1')
			str_copy.pop_back();

		const size_t length = str_copy.length();
		size_t i;
		for (i = 0; i < length && str_copy.at(i) != ' '; ++i);

		if (isAction())
			return str_copy.substr(i + 1);

		return str_copy;
	}

	std::string MessageLine::hatString() const {
		if (!isChannel())
			return "";

		if (!parent->cache.appearanceAllowEmptyHats)
			return hats == PingPong::Hat::None && !isAction()? " " : std::string(hats);

		return std::string(hats);
	}

	int MessageLine::getContinuation() {
#ifdef RERENDER_LINES
		render(nullptr);
#else
		if (rendered.empty()) {
			DBG("MessageLine::getContinuation(): rendered is empty");
			return 0;
		}
#endif
		return Line::getContinuation() + parent->getUI().renderer[getFormatKey()].positions.at("message");
	}

	int MessageLine::getNameIndex() {
#ifdef RERENDER_LINES
		render(nullptr);
#else
		if (rendered.empty()) {
			DBG("MessageLine::getNameIndex(): rendered is empty");
			return 0;
		}
#endif
		return Line::getContinuation() + parent->getUI().renderer[getFormatKey()].positions.at("nick");
	}

	NotificationType MessageLine::getNotificationType() const {
		if (Util::isHighlight(message, self, directOnly) || where == self)
			return NotificationType::Highlight;
		return NotificationType::Message;
	}

	std::string MessageLine::render(UI::Window *) {
		return parent->getUI().renderer(getFormatKey(), {
			{"raw_nick", name}, {"hats", hatString()},
			{"raw_message", PingPong::Util::irc2ansi(isAction()? trimmed(message) : message)}
		});
	}

	void MessageLine::onMouse(const Haunted::MouseReport &report) {
		//*
		if (report.action == Haunted::MouseAction::Up) {
			const int name_index = getNameIndex();
			if (name_index <= report.x && report.x < name_index + static_cast<int>(name.length())) {
				if (!server) {
					DBG("Can't query: server is null");
				} else {
					parent->getUI().focusWindow(parent->query(name, server));
				}

				return;
			}

			if (box) {
				// Compute the clicked character's index within the message.
				ssize_t n = -getContinuation();
				const ssize_t old_n = n;
				// I'm assuming there's nothing after the message in the format strings.
				ssize_t message_width = box->getPosition().width + n;
				n += report.x + report.y * message_width;
				DBG("baseContinuation[" << baseContinuation << "], -getContinuation[" << old_n << "], n[" << n << "]");
				ssize_t windex, sindex;
				std::tie(windex, sindex) = formicine::util::word_indices(message, n);
				std::string word = formicine::util::nth_word(message, windex);
				DBG("w[" << windex << "], s[" << sindex << "], word[" << word << "]");
				if (!word.empty() && word.front() == '#')
					PingPong::JoinCommand(server, word).send();
			}
		}
		//*/
	}
}
