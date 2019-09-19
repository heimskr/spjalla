#include "lines/mode.h"

namespace spjalla::lines {
	mode_line::operator std::string() const {
		if (!mset.is_type_valid())
			throw std::logic_error("Unknown mode type: " + std::to_string(static_cast<int>(mset.type)));

		const std::string prefix = lines::render_time(stamp) + lines::notice;
		const std::string suffix = " ("_d + std::string(mset) + ")"_d;

		if (mset.type == pingpong::modeset::mode_type::self)
			return prefix + "Your usermodes were adjusted" + suffix;

		return prefix + ansi::cyan(who->name) + " set modes for " + ansi::bold(where) + suffix;
	}
}
