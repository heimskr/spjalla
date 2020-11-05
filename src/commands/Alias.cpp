#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"
#include "spjalla/lines/Alias.h"

namespace Spjalla::Commands {
	void doAlias(Client &cli, const InputLine &il) {
		UI::Interface &ui = cli.getUI();

		if (il.args.empty()) {
			if (cli.aliasDB.empty()) {
				ui.warn("No aliases.");
			} else {
				for (auto & [key, expansion]: cli.aliasDB)
					ui.log(Lines::AliasLine(&cli, key, expansion));
			}

			return;
		}

		if (il.args.size() == 2 && il.first() == "-") {
			const std::string &key = il.args[1];
			if (cli.aliasDB.remove(key, true))
				ui.log("Alias " + ansi::bold(key) + " was removed.");
			else
				ui.warn("Alias " + ansi::bold(key) + " doesn't exist.");
			return;
		}

		try {
			const std::pair<std::string, std::string> apply_result = cli.aliasDB.applyLine(il.body);
			cli.aliasDB.writeDB();
			ui.log("Added alias " + ansi::bold(apply_result.first) + ".");
		} catch (const std::invalid_argument &err) {
			DBG("Couldn't parse alias insertion [" << il.body << "]: " << err.what());
			ui.warn("Invalid syntax for alias " + "\""_d + il.body + "\""_d);
		}
	}
}
