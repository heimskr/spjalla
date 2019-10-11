#include "pingpong/commands/join.h"
#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"

namespace spjalla::commands {
	void do_join(client &cli, pingpong::server *serv, const input_line &il) {
		const std::string &first = il.first();
		cli.wait_for_server(serv, pingpong::server::stage::ready, [=]() {
			pingpong::join_command(serv, first).send();
		});
	}
}
