#include "pingpong/core/ParseError.h"

#include "spjalla/core/Client.h"
#include "spjalla/lines/Mode.h"

namespace Spjalla::Lines {
	std::string ModeLine::render(UI::Window *) {
		if (!modeSet.isTypeValid())
			throw std::logic_error("Unknown mode type: " + std::to_string(static_cast<int>(modeSet.type)));

		const std::string prefix = Lines::notice;
		const std::string suffix = " ("_d + std::string(modeSet) + ")"_d;

		if (modeSet.type == PingPong::ModeSet::ModeType::Self && (where.empty() || where.front() != '#'))
			return prefix + "Your usermodes were adjusted" + suffix;

		const std::string modestr = modeSet.modeString();
		const std::string &extra = modeSet.extra;
		const std::string styled_name = parent->getUI().renderer.nick(who) + " ";

		if (!extra.empty()) {
			auto iter = verbs.find(modestr);
			if (iter != verbs.end()) {
				try {
					if (PingPong::Mask(extra).nick == self)
						return Lines::redNotice + styled_name + iter->second + " " + ansi::bold(extra);
				} catch (const PingPong::ParseError &err) {
					DBG("Parse error: " << err.what());
				}

				return prefix + styled_name + iter->second + " " + ansi::bold(extra);
			}
		}

		return prefix + styled_name + "set modes for " + parent->getUI().renderer.channel(where) + suffix;
	}

	std::unordered_map<std::string, std::string> ModeLine::verbs = {
		{"+b", "banned"},    {"-b", "unbanned"},
		{"+h", "halfopped"}, {"-h", "dehalfopped"},
		{"+o", "opped"},     {"-o", "deopped"},
		{"+v", "voiced"},    {"-v", "devoiced"},
	};
}
