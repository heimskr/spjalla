#include <chrono>
#include <ctime>

#include "spjalla/lines/line.h"
#include "lib/formicine/ansi.h"

namespace spjalla::lines {
	std::string notice = "\e[2m-\e[1m!\e[0;2m-\e[22m ";
	std::string red_notice = "\e[31;2m-\e[22m!\e[2m-\e[22;39m ";

	std::string render_time(long seconds) {
		if (seconds == 0)
			return "["_d + "__:__:__" + "]"_d;

		std::chrono::system_clock::time_point tpoint {std::chrono::duration<long>(seconds)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
		char str[64];
		std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
		return "["_d + str + "] "_d;
	}
}
