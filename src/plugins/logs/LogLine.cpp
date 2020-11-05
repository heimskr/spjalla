#include "pingpong/core/Util.h"
#include "spjalla/plugins/logs/LogLine.h"
#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins::Logs {
	std::string LogLine::render(UI::Window *) {
		return "───── "_d + "Log " + verb + " on " + ansi::bold(PingPong::Util::getDate(stamp)) + " ─────"_d;
	}
}
