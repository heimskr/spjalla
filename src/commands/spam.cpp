#include "spjalla/commands/command.h"
#include "spjalla/ui/interface.h"

namespace spjalla::commands {
	void do_spam(ui::interface &ui, const input_line &il) {
		long max = 64;

		if (!il.args.empty()) {
			char *ptr;
			const std::string &str = il.first();
			long parsed = strtol(str.c_str(), &ptr, 10);
			if (ptr != 1 + &*(str.end() - 1)) {
				ui.log(ansi::yellow("!!") + " Invalid number: \"" + il.first() + "\"");
			} else max = parsed;
		}

		for (long i = 1; i <= max; ++i)
			ui.log(std::to_string(i));
	}
}
