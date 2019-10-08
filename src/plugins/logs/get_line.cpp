#include <chrono>
#include <stdexcept>
#include <vector>

#include "pingpong/core/util.h"
#include "spjalla/lines/basic.h"
#include "spjalla/lines/line.h"
#include "spjalla/plugins/logs.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	std::unique_ptr<lines::line> logs_plugin::get_line(const std::string &str) {
		const size_t word_count = formicine::util::word_count(str);
		if (word_count < 2)
			throw std::invalid_argument("Log line is too short");

		std::string stamp_str = formicine::util::nth_word(str, 0, false);

		std::chrono::microseconds micros = parse_stamp(stamp_str);

		const std::string verb = formicine::util::nth_word(str, 1, false);

		// DBG("verb[" << verb << "], micros[" << micros.count() << "], stamp_str[" << stamp_str << "]");
		return std::make_unique<lines::basic_line>(
			"micros[" + std::to_string(micros.count()) + "], verb[" + verb + "]"
		, 0, std::chrono::duration_cast<pingpong::util::timetype>(micros).count());
	}
}
