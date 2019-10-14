#include "spjalla/lines/raw.h"

namespace spjalla::lines {
	raw_line::operator std::string() const {
		std::string out = lines::render_time(stamp);
		const char *indicator = is_out? "<< " : ">> ";
		if (is_bad)
			out += ansi::red(indicator);
		else if (is_out)
			out += ansi::gray(indicator);
		else
			out += ansi::dim(ansi::gray(indicator));
		return out + text;
	}
}
