#include "core/parse_error.h"

#include "lines/mode.h"

namespace spjalla::lines {
	mode_line::operator std::string() const {
		if (!mset.is_type_valid())
			throw std::logic_error("Unknown mode type: " + std::to_string(static_cast<int>(mset.type)));

		const std::string prefix  {lines::render_time(stamp) + lines::notice};
		const std::string suffix  {" ("_d + std::string(mset) + ")"_d};

		if (mset.type == pingpong::modeset::mode_type::self)
			return prefix + "Your usermodes were adjusted" + suffix;

		const std::string modestr = mset.mode_str();
		const std::string &extra = mset.extra;
		const std::string styled_name = ansi::cyan(who->name) + " ";

		if (!extra.empty()) {
			auto iter = verbs.find(modestr);
			if (iter != verbs.end()) {
				try {
					pingpong::mask mask {extra};
					if (mask.nick == who->serv->get_nick()) {
						return lines::render_time(stamp) + lines::red_notice + styled_name + iter->second + " " +
							ansi::bold(extra);
					}
				} catch (const pingpong::parse_error &) {}

				return prefix + styled_name + iter->second + " " + ansi::bold(extra);
			}
		}

		return prefix + styled_name + " set modes for " + ansi::bold(where) + suffix;
	}

	std::unordered_map<std::string, std::string> mode_line::verbs = {
		{"+b", "banned"},    {"-b", "unbanned"},
		{"+h", "halfopped"}, {"-h", "dehalfopped"},
		{"+o", "opped"},     {"-o", "deopped"},
		{"+v", "voiced"},    {"-v", "devoiced"},
	};
}
