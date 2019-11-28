#include "pingpong/commands/privmsg.h"

#include "spjalla/core/client.h"
#include "spjalla/core/util.h"

namespace spjalla {
	void client::add_input_listener() {
		ui.input->listen(haunted::ui::textinput::event::submit, [&](const haunted::ustring &ustr, int) -> void {
			if (ustr.empty())
				return;

			std::string str = ustr;
			ui.input->clear();

			input_line il = get_input_line(str);
			alias_db.expand(il);

			if (!before_input(il))
				return;

			if (il.is_command()) {
				try {
					if (!handle_line(il)) {
						// If the command isn't an exact match, try partial matches (e.g., "/j" for "/join").
						const std::string &cmd = il.command;

						std::vector<std::string> matches = command_matches(cmd);

						if (1 < matches.size()) {
							ui.log("Ambiguous command: /" + cmd);
							std::string joined;
							for (const std::string &match: matches)
								joined += "/" + match + " ";
							DBG("Matches: " << joined);
						} else if (matches.empty() || !handle_line("/" + matches[0] + " " + il.body)) {
							ui.log("Unknown command: /" + cmd);
						}
					}
				} catch (std::exception &err) {
					ui.log(err);
				}
			} else if (!ui.active_window->is_dead()) {
				if (std::shared_ptr<pingpong::channel> chan = ui.get_active_channel()) {
					pingpong::privmsg_command(chan, il.body).send();
				} else if (std::shared_ptr<pingpong::user> user = ui.get_active_user()) {
					pingpong::privmsg_command(user, il.body).send();
				} else {
					no_channel();
				}
			}

			after_input(il);
		});
	}

	std::vector<std::string> client::command_matches(const std::string &command_name) {
		std::vector<std::string> matches;

		const size_t command_length = command_name.length();

		for (const std::pair<std::string, commands::command> &pair: command_handlers) {
			const std::string &candidate_name = pair.first;
			if (candidate_name.substr(0, command_length) == command_name)
				matches.push_back(candidate_name);
		}

		return matches;
	}

	input_line client::get_input_line(const std::string &str) const {
		if (!str.empty() && ui.active_window == ui.status_window && str.front() != '/')
			return input_line("/" + str);

		return input_line(str);
	}

	bool client::handle_line(const input_line &il) {
		const int nargs = static_cast<int>(il.args.size());
		const std::string &name = il.command;

		auto range = command_handlers.equal_range(name);
		// Quit if there are no matching handlers.
		if (range.first == range.second)
			return false;

		for (auto it = range.first; it != range.second; ++it) {
			auto & [min, max, needs_serv, fn, comp_fn, suggestion_fns] = it->second;
			if (max == 0 && nargs != 0) {
				ui.warn("/" + name + " doesn't accept any arguments.");
			} else if (min == max && nargs != min) {
				ui.warn("/" + name + " expects " + std::to_string(min) + " argument" + (min == 1? "." : "s."));
			} else if (nargs < min) {
				ui.warn("/" + name + " expects at least " + std::to_string(min) + " argument" + (min == 1? "." : "s."));
			} else if (max != -1 && max < nargs) {
				ui.warn("/" + name + " expects at most " + std::to_string(max) + " argument" + (min == 1? "." : "s."));
			} else if (needs_serv && !irc.active_server) {
				ui.warn("No server is selected.");
			} else {
				fn(active_server(), il);
			}
		}

		return true;
	}
}
