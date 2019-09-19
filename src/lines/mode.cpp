#include "lines/mode.h"

namespace spjalla::lines {
	mode_line::operator std::string() const {
		if (!mset.is_type_valid())
			throw std::logic_error("Unknown mode type: " + std::to_string(static_cast<int>(mset.type)));

		const std::string prefix = lines::render_time(stamp) + lines::notice;
		const std::string suffix = " were adjusted " + "("_d + ansi::bold(mset) + ")"_d;

		if (mset.type == pingpong::modeset::mode_type::self)
			return prefix + "Your usermodes" + suffix;

		return prefix + "Modes for " + ansi::bold(where) + suffix;
	}
}
