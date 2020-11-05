#include <chrono>
#include <stdexcept>
#include <vector>

#include "pingpong/core/Hats.h"
#include "pingpong/core/Util.h"
#include "pingpong/core/ModeSet.h"

#include "spjalla/core/Client.h"
#include "spjalla/lines/Basic.h"
#include "spjalla/lines/Join.h"
#include "spjalla/lines/Kick.h"
#include "spjalla/lines/Mode.h"
#include "spjalla/lines/NickChange.h"
#include "spjalla/lines/Notice.h"
#include "spjalla/lines/Part.h"
#include "spjalla/lines/Privmsg.h"
#include "spjalla/lines/Quit.h"
#include "spjalla/lines/Topic.h"

#include "spjalla/plugins/Logs.h"
#include "spjalla/plugins/logs/LogLine.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins::Logs {
	std::unique_ptr<Lines::Line> LogsPlugin::getLine(const LogPair &pair, const std::string &str, bool autoclean) {
		const size_t word_count = formicine::util::word_count(str);
		if (word_count < 2)
			throw std::invalid_argument("Log line is too short");

		const std::chrono::microseconds micros = parseStamp(formicine::util::nth_word(str, 0, false));
		const long stamp = std::chrono::duration_cast<PingPong::Util::TimeType>(micros).count();

		const std::string verb    = formicine::util::nth_word(str, 1, false);
		const std::string subject = formicine::util::nth_word(str, 2, false);
		const std::string object  = formicine::util::nth_word(str, 3, false);

		if (verb == "msg") {
			Lines::PrivmsgLine *new_line = new Lines::PrivmsgLine(parent, subject, pair.second, object,
				str.substr(str.find(':') + 1), stamp);
			new_line->server = pair.first;
			new_line->box = parent->getUI().getWindow(pair.first->getChannel(pair.second), false);
			return std::unique_ptr<Lines::PrivmsgLine>(new_line);
		} else if (verb == "notice") {
			return std::make_unique<Lines::NoticeLine>(parent, subject, pair.second, object,
				str.substr(str.find(':') + 1), stamp);
		} else if (verb == "created" || verb == "opened" || verb == "closed") {
			return autoclean? nullptr : std::make_unique<LogLine>(parent, verb, stamp);
		} else if (verb == "join") {
			return std::make_unique<Lines::JoinLine>(parent, pair.second, subject, stamp);
		} else if (verb == "part") {
			return std::make_unique<Lines::PartLine>(parent, pair.second, subject, str.substr(str.find(':') + 1),
				stamp);
		} else if (verb == "mode") {
			PingPong::ModeSet mset(PingPong::ModeSet::ModeType::Channel, formicine::util::nth_word(str, 4, false),
				formicine::util::skip_words(str, 5));
			return std::make_unique<Lines::ModeLine>(parent, mset, pair.second, subject, object, stamp);
		} else if (verb == "topic_is") {
			return std::make_unique<Lines::TopicLine>(parent, "", pair.second, str.substr(str.find(':') + 1), stamp);
		} else if (verb == "topic_set") {
			return std::make_unique<Lines::TopicLine>(parent, subject, pair.second, str.substr(str.find(':') + 1),
				stamp);
		} else if (verb == "quit") {
			return std::make_unique<Lines::QuitLine>(parent, subject, str.substr(str.find(':') + 1), stamp);
		} else if (verb == "nick") {
			return std::make_unique<Lines::NickChangeLine>(parent, subject, object, stamp);
		} else if (verb == "kick") {
			return std::make_unique<Lines::KickLine>(parent, pair.first->getChannel(pair.second, true), subject, object,
				str.substr(str.find(':') + 1), stamp);
		}

		DBG("Line: " << "["_d << str << "]"_d);

		return std::make_unique<Lines::BasicLine>(parent, "micros[" + std::to_string(micros.count()) + "], verb["
			+ verb + "]", 0, stamp);
	}
}
