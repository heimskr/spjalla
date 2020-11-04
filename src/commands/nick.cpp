#include "pingpong/commands/Nick.h"
#include "pingpong/core/Server.h"
#include "spjalla/commands/Command.h"
#include "spjalla/ui/Interface.h"

namespace Spjalla::Commands {
	void doNick(UI::Interface &ui, PingPong::Server *server, const InputLine &il) {
		if (il.args.size() == 0)
			ui.log("Current nick: " + server->getNick());
		else
			PingPong::NickCommand(server, il.first()).send();
	}
}
