#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"

namespace Spjalla::Commands {
	void doConnect(Client &client, const InputLine &il) {
		const std::string &where = il.first();
		const std::string nick = il.args.size() > 1?
			il.args[1] : client.configs.get("server", "default_nick").string_();
		UI::Interface &ui = client.getUI();

		std::string hostname;
		long port = 0;

		std::tie(hostname, port) = client.getIRC().connect(where, nick, 6667, [&](const std::function<void()> &fn) {
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
