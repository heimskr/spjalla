#include "pingpong/core/Server.h"
#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"

namespace Spjalla::Commands {
	void doDisconnect(Client &client, PingPong::Server *server, const InputLine &il) {
		if (il.args.empty()) {
			server->quit(il.body);
			return;
		}

		const std::string first = il.first();
		const std::string reason = formicine::util::skip_words(il.body);

		for (PingPong::Server *subserver: client.getIRC().serverOrder) {
			if (subserver->id == first) {
				subserver->quit(reason);
				return;
			}
		}

		bool quit_any = false;
		for (PingPong::Server *subserver: client.getIRC().serverOrder) {
			if (subserver->hostname == first) {
				subserver->quit(reason);
				quit_any = true;
			}
		}

		if (!quit_any)
			client.getUI().warn("Quit: there is no server " + ansi::bold(first) + ".");
	}
}
