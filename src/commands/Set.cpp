#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"
#include "spjalla/lines/ConfigKey.h"
#include "spjalla/lines/ConfigGroup.h"

namespace Spjalla::Commands {
	void doSet(Client &client, const InputLine &il) {
		Spjalla::UI::Interface &ui = client.getUI();
		client.configs.readIfEmpty(DEFAULT_CONFIG_DB);

		Config::Database::GroupMap with_defaults = client.configs.withDefaults();

		if (il.args.empty()) {
			for (const auto &gpair: with_defaults) {
				ui.log(Lines::ConfigGroupLine(&client, gpair.first));
				for (const auto &spair: gpair.second)
					ui.log(Lines::ConfigKeyLine(&client, spair));
			}

			return;
		}

		const std::string &first = il.first();

		std::pair<std::string, std::string> parsed;

		if (first.find('.') == std::string::npos) {
			parsed.second = first;
			for (const auto &gpair: with_defaults) {
				if (gpair.second.count(first) == 1) {
					if (!parsed.first.empty()) {
						ui.warn("Multiple groups contain the key " + ansi::bold(first) + ".");
						return;
					}

					parsed.first = gpair.first;
				}
			}
		} else {
			try {
				parsed = Config::Database::parsePair(first);
			} catch (const std::invalid_argument &) {
				ui.warn("Couldn't parse setting " + ansi::bold(first));
				return;
			}
		}

		if (il.args.size() == 1) {
			try {
				const Config::Value &value = client.configs.getPair(parsed);
				ui.log(Lines::ConfigKeyLine(&client, parsed.first + "." + parsed.second, value, false));
			} catch (const std::out_of_range &) {
				ui.log("No configuration option for " + ansi::bold(first) + ".");
			}
		} else {
			const std::string joined = formicine::util::join(il.args.begin() + 1, il.args.end());

			// Special case: setting a value to "-" removes it from the database.
			if (joined == "-") {
				if (client.configs.remove(parsed.first, parsed.second, true, true)) {
					ui.log("Removed " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + ".");
				} else {
					ui.log("Couldn't find " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + ".");
				}
			} else {
				Config::ValueType type = Config::Database::getValueType(joined);
				if (type == Config::ValueType::Invalid) {
					client.configs.insert(parsed.first, parsed.second, {joined});
				} else {
					client.configs.insertAny(parsed.first, parsed.second, joined);
				}

				ui.log("Set " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + " to " +
					ansi::bold(joined) + ".");
			}
		}
	}
}
