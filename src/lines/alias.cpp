#include "spjalla/lines/alias.h"

namespace spjalla::lines {
	alias_line::operator std::string() const {
		return lines::render_time(stamp) + key + " = "_d + ansi::bold(expansion);
	}
}
