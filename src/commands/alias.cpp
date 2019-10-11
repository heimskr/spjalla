#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"
#include "spjalla/lines/alias.h"

namespace spjalla::commands {
	void do_alias(client &cli, const input_line &il) {
		ui::interface &ui = cli.get_ui();

		if (il.args.empty()) {
			if (cli.alias_db.empty()) {
				ui.warn("No aliases.");
			} else {
				for (auto & [key, expansion]: cli.alias_db)
					ui.log(lines::alias_line(&cli, key, expansion));
			}

			return;
		}

		if (il.args.size() == 2 && il.first() == "-") {
			const std::string &key = il.args[1];
			if (cli.alias_db.remove(key, true))
				ui.log("Alias " + ansi::bold(key) + " was removed.");
			else
				ui.warn("Alias " + ansi::bold(key) + " doesn't exist.");
			return;
		}

		try {
			const std::pair<std::string, std::string> apply_result = cli.alias_db.apply_line(il.body);
			cli.alias_db.write_db();
			ui.log("Added alias " + ansi::bold(apply_result.first) + ".");
		} catch (const std::invalid_argument &err) {
			DBG("Couldn't parse alias insertion [" << il.body << "]: " << err.what());
			ui.warn("Invalid syntax for alias " + "\""_d + il.body + "\""_d);
		}
	}
}
