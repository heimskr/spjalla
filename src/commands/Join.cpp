#include "pingpong/commands/Join.h"
#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"

namespace Spjalla::Commands {
	void doJoin(Client &cli, PingPong::Server *server, const InputLine &il) {
		const std::string &first = il.first();
		cli.waitForServer(server, PingPong::Server::Stage::Ready, [=]() {
			PingPong::JoinCommand(server, first).send();
		});
	}
}
