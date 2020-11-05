#include "pingpong/commands/Part.h"
#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"

namespace Spjalla::Commands {
	void doPart(Client &cli, PingPong::Server *server, const InputLine &il) {
		std::shared_ptr<PingPong::Channel> active_channel = cli.getUI().getActiveChannel();

		if ((il.args.empty() || il.first()[0] != '#') && !active_channel) {
			cli.noChannel();
		} else if (il.args.empty()) {
			PingPong::PartCommand(server, active_channel).send();
		} else if (il.first()[0] != '#') {
			PingPong::PartCommand(server, active_channel, il.body).send();
		} else if (std::shared_ptr<PingPong::Channel> cptr = server->getChannel(il.first())) {
			PingPong::PartCommand(server, cptr, il.rest()).send();
		} else {
			cli.getUI().log(il.first() + ": no such channel.");
		}
	}
}
