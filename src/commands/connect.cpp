#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"

namespace spjalla::commands {
	void do_connect(client &cli, const input_line &il) {
		const std::string &where = il.first();
		const std::string nick = il.args.size() > 1? il.args[1] : cli.configs.get("server", "default_nick").string_();
		ui::interface &ui = cli.get_ui();

		std::string hostname;
		long port = 0;

		std::tie(hostname, port) = cli.get_irc().connect(where, nick, 6667, [&](const std::function<void()> &fn) {
			try {
				fn();
			} catch (const std::exception &err) {
				ui.warn("Couldn't connect to " + ansi::bold(hostname) + " on port " + ansi::bold(std::to_string(port))
					+ ": " + err.what());
			}
		});

		ui.log("Connecting to " + ansi::bold(where) + " on port " + ansi::bold(std::to_string(port)) + "...");
	}
}
