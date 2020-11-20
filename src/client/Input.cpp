#include "pingpong/commands/Privmsg.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"

namespace Spjalla {
	void Client::addInputListener() {
		ui.input->listen(Haunted::UI::TextInput::Event::Submit, [&](const Haunted::ustring &ustr, int) -> void {
			if (ustr.empty())
				return;

			std::string str = ustr;
			ui.input->clear();

			InputLine il = getInputLine(str);
			aliasDB.expand(il);

			if (!beforeInput(il))
				return;

			if (il.isCommand()) {
				try {
					if (!handleLine(il)) {
						// If the command isn't an exact match, try partial matches (e.g., "/j" for "/join").
						const std::string &cmd = il.command;

						std::vector<std::string> matches = commandMatches(cmd);

						if (1 < matches.size()) {
							ui.log("Ambiguous command: /" + cmd);
							std::string joined;
							for (const std::string &match: matches)
								joined += "/" + match + " ";
							DBG("Matches: " << joined);
						} else if (matches.empty() || !handleLine("/" + matches[0] + " " + il.body)) {
							ui.log("Unknown command: /" + cmd);
						}
					}
				} catch (std::exception &err) {
					ui.log(err);
				}
			} else if (!ui.activeWindow->isDead()) {
				if (std::shared_ptr<PingPong::Channel> chan = ui.getActiveChannel()) {
					PingPong::PrivmsgCommand(chan, il.body).send();
				} else if (std::shared_ptr<PingPong::User> user = ui.getActiveUser()) {
					PingPong::PrivmsgCommand(user, il.body).send();
				} else {
					noChannel();
				}
			}

			afterInput(il);
		});
	}

	std::vector<std::string> Client::commandMatches(const std::string &command_name) {
		std::vector<std::string> matches;

		const size_t command_length = command_name.length();

		for (const std::pair<std::string, Commands::Command> &pair: commandHandlers) {
			const std::string &candidate_name = pair.first;
			if (candidate_name.substr(0, command_length) == command_name)
				matches.push_back(candidate_name);
		}

		return matches;
	}

	InputLine Client::getInputLine(const std::string &str) const {
		if (!str.empty() && ui.activeWindow == ui.statusWindow && str.front() != '/')
			return InputLine("/" + str);

		return InputLine(str);
	}

	bool Client::handleLine(const InputLine &il) {
		const int nargs = static_cast<int>(il.args.size());
		const std::string &name = il.command;

		auto range = commandHandlers.equal_range(name);
		// Quit if there are no matching handlers.
		if (range.first == range.second)
			return false;

		for (auto it = range.first; it != range.second; ++it) {
			auto &[min, max, needs_serv, fn, comp_fn, suggestion_fns] = it->second;
			if (max == 0 && nargs != 0)
				ui.warn("/" + name + " doesn't accept any arguments.");
			else if (min == max && nargs != min)
				ui.warn("/" + name + " expects " + std::to_string(min) + " argument" + (min == 1? "." : "s."));
			else if (nargs < min)
				ui.warn("/" + name + " expects at least " + std::to_string(min) + " argument" + (min == 1? "." : "s."));
			else if (max != -1 && max < nargs)
				ui.warn("/" + name + " expects at most " + std::to_string(max) + " argument" + (min == 1? "." : "s."));
			else if (needs_serv && !irc.activeServer)
				ui.warn("No server is selected.");
			else if (fn(activeServer(), il))
				break;
		}

		return true;
	}
}
