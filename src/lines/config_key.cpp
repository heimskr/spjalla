#include "spjalla/lines/config_key.h"

namespace spjalla::lines {
	config_key_line::operator std::string() const {
		return lines::render_time(stamp) + (indent? "    " : "") + key + " = "_d + ansi::bold(std::string(value));
	}
}
