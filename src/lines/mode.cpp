#include "pingpong/core/parse_error.h"

#include "spjalla/core/client.h"
#include "spjalla/lines/mode.h"

namespace spjalla::lines {
	std::string mode_line::render(ui::window *) {

		if (!mset.is_type_valid())
			throw std::logic_error("Unknown mode type: " + std::to_string(static_cast<int>(mset.type)));

		const std::string prefix  {lines::notice};
		const std::string suffix  {" ("_d + std::string(mset) + ")"_d};

		if (mset.type == pingpong::modeset::mode_type::self && (where.empty() || where.front() != '#'))
			return prefix + "Your usermodes were adjusted" + suffix;

		const std::string modestr = mset.mode_str();
		const std::string &extra = mset.extra;
		const std::string styled_name =
			parent->get_ui().render.nick(who, where, ui::renderer::nick_situation::normal, true) + " ";

		if (!extra.empty()) {
			auto iter = verbs.find(modestr);
			if (iter != verbs.end()) {
				try {
					pingpong::mask mask {extra};
					if (mask.nick == self) {
						return lines::red_notice + styled_name + iter->second + " " +
							ansi::bold(extra);
					}
				} catch (const pingpong::parse_error &) {}

				return prefix + styled_name + iter->second + " " + ansi::bold(extra);
			}
		}

		return prefix + styled_name + "set modes for " + parent->get_ui().render.channel(where) + suffix;
	}

	std::unordered_map<std::string, std::string> mode_line::verbs = {
		{"+b", "banned"},    {"-b", "unbanned"},
		{"+h", "halfopped"}, {"-h", "dehalfopped"},
		{"+o", "opped"},     {"-o", "deopped"},
		{"+v", "voiced"},    {"-v", "devoiced"},
	};
}
