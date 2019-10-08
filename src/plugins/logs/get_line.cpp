#include <chrono>
#include <stdexcept>
#include <vector>

#include "pingpong/core/hats.h"
#include "pingpong/core/util.h"

#include "spjalla/lines/basic.h"
#include "spjalla/lines/privmsg.h"

#include "spjalla/plugins/logs.h"

#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	std::unique_ptr<lines::line> logs_plugin::get_line(const log_pair &pair, const std::string &str) {
		const size_t word_count = formicine::util::word_count(str);
		if (word_count < 2)
			throw std::invalid_argument("Log line is too short");

		const std::chrono::microseconds micros = parse_stamp(formicine::util::nth_word(str, 0, false));
		const long stamp = std::chrono::duration_cast<pingpong::util::timetype>(micros).count();

		const std::string verb = formicine::util::nth_word(str, 1, false);

		if (verb == "msg") {
			return std::make_unique<lines::privmsg_line>(formicine::util::nth_word(str, 2, false), pair.second,
				formicine::util::nth_word(str, 3, false), str.substr(str.find(':') + 1), stamp);
		} else if (verb == "created" || verb == "opened" || verb == "closed") {
			return std::make_unique<lines::basic_line>(lines::notice + "Log " + verb + " on " +
				ansi::bold(pingpong::util::get_date(stamp)), 0, stamp);
		}

		return std::make_unique<lines::basic_line>(
			"micros[" + std::to_string(micros.count()) + "], verb[" + verb + "]"
		, 0, stamp);
	}
}
