#include "spjalla/commands/Command.h"
#include "spjalla/ui/Interface.h"
#include "spjalla/core/Util.h"

namespace Spjalla::Commands {
	void doMove(UI::Interface &ui, const InputLine &il) {
		long parsed;
		const std::string first = il.first();
		if (!formicine::util::parse_long(first, parsed)) {
			ui.warn("Invalid number: " + "\""_bd + ansi::bold(first) + "\""_bd);
			return;
		}

		ui.moveWindow(ui.getActiveWindow(), std::max(0L, parsed - 1));
	}
}
