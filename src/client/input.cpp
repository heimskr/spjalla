#include "pingpong/commands/privmsg.h"

#include "core/client.h"
#include "core/sputil.h"

namespace spjalla {
	void client::add_input_listener() {
		ui.input->listen(haunted::ui::textinput::event::submit, [&](const haunted::superstring &sstr, int) -> void {
			if (sstr.empty()) return;
			std::string str = sstr.str();
			ui.input->clear();

			input_line il = get_input_line(str);

			if (!before_input(il))
				return;

			if (il.is_command()) {
				try {
					if (!handle_line(il)) {
						// If the command isn't an exact match, try partial matches (e.g., "/j" for "/join").
						const std::string &cmd = il.command;
						size_t cmd_length = cmd.length();

						std::vector<std::string> matches;

						for (std::pair<std::string, command_tuple> pair: command_handlers) {
							const std::string &candidate_name = pair.first;
							if (candidate_name.substr(0, cmd_length) == cmd)
								matches.push_back(candidate_name);
						}

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
					pingpong::privmsg_command(chan, str).send();
				} else if (std::shared_ptr<pingpong::user> user = ui.get_active_user()) {
					pingpong::privmsg_command(user, str).send();
				} else {
					no_channel();
				}
			}

			after_input(il);
		});
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
			auto [min, max, needs_serv, fn, comp_fn] = it->second;
			if (max == 0 && nargs != 0) {
				DBG("/" << name << " doesn't accept any arguments.");
			} else if (min == max && nargs != min) {
				DBG("/" << name << " expects " << min << " argument" << (min == 1? "." : "s."));
			} else if (nargs < min) {
				DBG("/" << name << " expects at least " << std::to_string(min) << " argument"
				      << (min == 1? "." : "s."));
			} else if (max != -1 && max < nargs) {
				DBG("/" << name << " expects at most " << std::to_string(max) << " argument"
				      << (min == 1? "." : "s."));
			} else if (needs_serv && !irc.active_server) {
				ui.log(lines::red_notice + "No server is selected.");
			} else {
				fn(irc.active_server, il);
			}
		}

		return true;
	}

	void client::tab_complete() {
		std::string text = ui.input->get_text();
		if (text.empty())
			return;

		size_t cursor = ui.input->get_cursor();

		if (ui.active_window == ui.status_window && text.front() != '/') {
			text.insert(0, "/");
			ui.input->set_text(text);
			ui.input->move_to(++cursor);
			ui.input->jump_cursor();
		}

		input_line il = get_input_line(text);
		ssize_t windex = util::word_index(text, cursor);
		DBG("Command[" << il.command << "], windex[" << windex << "]");

	}
}
