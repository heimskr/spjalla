#include "spjalla/commands/command.h"
#include "spjalla/ui/interface.h"
#include "spjalla/core/util.h"

namespace spjalla::commands {
	void do_move(ui::interface &ui, const input_line &il) {
		long parsed;
		const std::string first = il.first();
		if (!util::parse_long(first, parsed)) {
			ui.warn("Invalid number: " + "\""_bd + ansi::bold(first) + "\""_bd);
			return;
		}

		ui.move_window(ui.get_active_window(), std::max(0L, parsed - 1));
	}
}
