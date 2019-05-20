#include <iostream>
#include <string>
#include <thread>

#include "commands/all.h"
#include "core/debug.h"
#include "core/defs.h"
#include "core/irc.h"
#include "core/server.h"
#include "lib/ansi.h"

#include "core/spjalla.h"
#include "core/input_line.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	void input_worker(server_ptr serv) {
		std::string in;
		while (std::getline(std::cin, in)) {
			input_line il = input_line(in);

			if (il.is_command()) {
				try {
					if (il.command == "nick") {
						if (il.args.size() != 1)
							YIKES("/nick expects one argument.");
						nick_command(serv, il.args[0]).send();
					} else if (il.command == "join") {
						if (il.args.size() != 1)
							YIKES("/join expects one argument.");
						join_command(serv, il.args[0]).send();
					} else std::cerr << "Unknown command: /" << il.command << std::endl;
				} catch (std::exception &err) {
					YIKES(err.what());
				}
			} else {

			}
		}
	}
}

int main(int argc, char **argv) {
	irc instance;

	string hostname;

	hostname = 1 < argc? argv[1] : "localhost";
	server serv(instance, hostname);
	serv.start();
	serv.set_nick("pingpong");
	std::thread input(&input_worker, &serv);
	serv.server_thread->join();
	input.join();
}
