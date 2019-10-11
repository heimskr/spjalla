#include "pingpong/commands/nick.h"
#include "pingpong/core/server.h"
#include "spjalla/commands/command.h"
#include "spjalla/ui/interface.h"

namespace spjalla::commands {
	void do_nick(ui::interface &ui, pingpong::server *serv, const input_line &il) {
		if (il.args.size() == 0)
			ui.log("Current nick: " + serv->get_nick());
		else
			pingpong::nick_command(serv, il.first()).send();
	}
}
