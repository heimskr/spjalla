#include "pingpong/core/server.h"
#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"

namespace spjalla::commands {
	void do_disconnect(client &cli, pingpong::server *serv, const input_line &il) {
		if (il.args.empty()) {
			serv->quit(il.body);
			return;
		}

		const std::string first = il.first();
		const std::string reason = util::skip_words(il.body);

		for (pingpong::server *subserv: cli.get_irc().server_order) {
			if (subserv->id == first) {
				subserv->quit(reason);
				return;
			}
		}

		bool quit_any = false;
		for (pingpong::server *subserv: cli.get_irc().server_order) {
			if (subserv->hostname == first) {
				subserv->quit(reason);
				quit_any = true;
			}
		}

		if (!quit_any)
			cli.get_ui().warn("Quit: there is no server " + ansi::bold(first) + ".");
	}
}
