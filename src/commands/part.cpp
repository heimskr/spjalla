#include "pingpong/commands/part.h"
#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"

namespace spjalla::commands {
	void do_part(client &cli, pingpong::server *serv, const input_line &il) {
		std::shared_ptr<pingpong::channel> active_channel = cli.get_ui().get_active_channel();

		if ((il.args.empty() || il.first()[0] != '#') && !active_channel) {
			cli.no_channel();
		} else if (il.args.empty()) {
			pingpong::part_command(serv, active_channel).send();
		} else if (il.first()[0] != '#') {
			pingpong::part_command(serv, active_channel, il.body).send();
		} else if (std::shared_ptr<pingpong::channel> cptr = serv->get_channel(il.first())) {
			pingpong::part_command(serv, cptr, il.rest()).send();
		} else {
			cli.get_ui().log(il.first() + ": no such channel.");
		}
	}
}
