#include <chrono>
#include <ctime>

#include "pingpong/core/util.h"
#include "spjalla/lines/line.h"
#include "lib/formicine/ansi.h"

namespace spjalla::lines {
	std::string notice        = "\e[2m-\e[1m!\e[0;2m-\e[22m ";
	std::string red_notice    = "\e[31;2m-\e[22m!\e[2m-\e[22;39m ";
	std::string yellow_notice = "\e[33;2m-\e[22m!\e[2m-\e[22;39m ";

	std::string render_time(long stamp, bool with_ansi) {
		if (stamp == 0)
			return with_ansi? "["_d + "__:__:__" + "]"_d : "[__:__:__]";

		std::chrono::system_clock::time_point tpoint {pingpong::util::timetype(stamp)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
		char str[64];
		std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
		return with_ansi? "["_d + str + "] "_d : "[" + std::string(str) + "]";
	}

	long now() {
		return pingpong::util::timestamp();
	}
}
