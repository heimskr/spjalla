#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"
#include "spjalla/lines/config_key.h"
#include "spjalla/lines/config_group.h"

namespace spjalla::commands {
	void do_set(client &cli, const input_line &il) {
		spjalla::ui::interface &ui = cli.get_ui();
		cli.configs.read_if_empty(DEFAULT_CONFIG_DB);

		config::database::groupmap with_defaults = cli.configs.with_defaults();

		if (il.args.empty()) {
			for (const auto &gpair: with_defaults) {
				ui.log(lines::config_group_line(gpair.first));
				for (const auto &spair: gpair.second)
					ui.log(lines::config_key_line(spair));
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
				parsed = config::database::parse_pair(first);
			} catch (const std::invalid_argument &) {
				ui.warn("Couldn't parse setting " + ansi::bold(first));
				return;
			}
		}

		if (il.args.size() == 1) {
			try {
				const config::value &value = cli.configs.get_pair(parsed);
				ui.log(lines::config_key_line(parsed.first + "." + parsed.second, value, false));
			} catch (const std::out_of_range &) {
				ui.log("No configuration option for " + ansi::bold(first) + ".");
			}
		} else {
			const std::string joined = formicine::util::join(il.args.begin() + 1, il.args.end());

			// Special case: setting a value to "-" removes it from the database.
			if (joined == "-") {
				if (cli.configs.remove(parsed.first, parsed.second, true, true)) {
					ui.log("Removed " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + ".");
				} else {
					ui.log("Couldn't find " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + ".");
				}
			} else {
				config::value_type type = config::database::get_value_type(joined);
				if (type == config::value_type::invalid) {
					cli.configs.insert(parsed.first, parsed.second, {joined});
				} else {
					cli.configs.insert_any(parsed.first, parsed.second, joined);
				}

				ui.log("Set " + ansi::bold(parsed.first) + "."_bd + ansi::bold(parsed.second) + " to " +
					ansi::bold(joined) + ".");
			}
		}
	}
}
