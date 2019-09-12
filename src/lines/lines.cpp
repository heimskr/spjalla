#include <chrono>
#include <ctime>

#include "lines/lines.h"
#include "formicine/ansi.h"

namespace spjalla::lines {
	std::string render_time(long seconds) {
		if (seconds == 0)
			return "["_d + "__:__:__" + "]"_d;

		std::chrono::system_clock::time_point tpoint {std::chrono::duration<long>(seconds)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
		std::string str(64, '\0');
		std::strftime(&str[0], str.size(), "%H:%M:%S", std::localtime(&time));
		return "["_d + str + "] "_d;
	}
}