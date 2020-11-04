#include <chrono>
#include <stdexcept>
#include <vector>

#include "pingpong/core/Hats.h"
#include "pingpong/core/Util.h"
#include "pingpong/core/ModeSet.h"

#include "spjalla/core/Client.h"
#include "spjalla/lines/basic.h"
#include "spjalla/lines/join.h"
#include "spjalla/lines/kick.h"
#include "spjalla/lines/mode.h"
#include "spjalla/lines/nick_change.h"
#include "spjalla/lines/Notice.h"
#include "spjalla/lines/part.h"
#include "spjalla/lines/Privmsg.h"
#include "spjalla/lines/quit.h"
#include "spjalla/lines/topic.h"

#include "spjalla/plugins/Logs.h"
#include "spjalla/plugins/logs/log_line.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins::logs {
	std::unique_ptr<lines::line> logs_plugin::get_line(const LogPair &pair, const std::string &str, bool autoclean) {
		const size_t word_count = formicine::util::word_count(str);
		if (word_count < 2)
			throw std::invalid_argument("Log line is too short");

		const std::chrono::microseconds micros = parse_stamp(formicine::util::nth_word(str, 0, false));
		const long stamp = std::chrono::duration_cast<PingPong::Util::TimeType>(micros).count();

		const std::string verb    = formicine::util::nth_word(str, 1, false);
		const std::string subject = formicine::util::nth_word(str, 2, false);
		const std::string object  = formicine::util::nth_word(str, 3, false);

		if (verb == "msg") {
			lines::privmsg_line *new_line = new lines::privmsg_line(parent, subject, pair.second, object,
				str.substr(str.find(':') + 1), stamp);
			new_line->serv = pair.first;
			new_line->box = parent->getUI().get_window(pair.first->getChannel(pair.second), false);
			return std::unique_ptr<lines::privmsg_line>(new_line);
		} else if (verb == "notice") {
			return std::make_unique<lines::notice_line>(parent, subject, pair.second, object,
				str.substr(str.find(':') + 1), stamp);
		} else if (verb == "created" || verb == "opened" || verb == "closed") {
			return autoclean? nullptr : std::make_unique<log_line>(parent, verb, stamp);
		} else if (verb == "join") {
			return std::make_unique<lines::join_line>(parent, pair.second, subject, stamp);
		} else if (verb == "part") {
			return std::make_unique<lines::part_line>(parent, pair.second, subject, str.substr(str.find(':') + 1),
				stamp);
		} else if (verb == "mode") {
			PingPong::ModeSet mset {PingPong::ModeSet::mode_type::channel, formicine::util::nth_word(str, 4, false),
				formicine::util::skip_words(str, 5)};
			return std::make_unique<lines::mode_line>(parent, mset, pair.second, subject, object, stamp);
		} else if (verb == "topic_is") {
			return std::make_unique<lines::topic_line>(parent, "", pair.second, str.substr(str.find(':') + 1), stamp);
		} else if (verb == "topic_set") {
			return std::make_unique<lines::topic_line>(parent, subject, pair.second, str.substr(str.find(':') + 1),
				stamp);
		} else if (verb == "quit") {
			return std::make_unique<lines::quit_line>(parent, subject, str.substr(str.find(':') + 1), stamp);
		} else if (verb == "nick") {
			return std::make_unique<lines::nick_change_line>(parent, subject, object, stamp);
		} else if (verb == "kick") {
			return std::make_unique<lines::kick_line>(parent, pair.first->getChannel(pair.second, true), subject,
				object, str.substr(str.find(':') + 1), stamp);
		}

		DBG("Line: " << "["_d << str << "]"_d);

		return std::make_unique<lines::basic_line>(parent,
			"micros[" + std::to_string(micros.count()) + "], verb[" + verb + "]", 0, stamp);
	}
}
