#include "pingpong/core/Util.h"
#include "spjalla/plugins/logs/log_line.h"
#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins::logs {
	std::string log_line::render(UI::Window *) {
		return "───── "_d + "Log " + verb + " on " + ansi::bold(PingPong::Util::getDate(stamp)) + " ─────"_d;
	}
}
