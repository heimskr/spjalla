#include <iostream>
#include <string>
#include <thread>

#include "commands/all.h"
#include "core/debug.h"
#include "core/defs.h"
#include "core/irc.h"
#include "core/server.h"
#include "lib/ansi.h"
#include "events/event.h"
#include "events/message.h"
#include "messages/numeric.h"
#include "messages/ping.h"

#include "core/client.h"
#include "core/input_line.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	void client::input_worker(server_ptr serv) {
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
					} else if (il.command == "quit") {
						if (il.args.size() == 0) {
							quit_command(serv).send();
						} else {
							quit_command(serv, il.body).send();
						}
					} else if (il.command == "msg") {
						if (il.args.size() < 2) {
							YIKES("/msg expects at least two arguments.");
						} else {
							msg_command(serv, il.args[0], il.rest()).send();
						}
					} else if (il.command == "chans") {
						std::cout << "Channels:";
						for (const channel &chan: serv->channels)
							std::cout << " " << std::string(chan);
						std::cout << "\n";
					} else if (il.command == "quote") {
						serv->quote(il.body);
					} else if (il.command == "part") {
						if (il.args.size() < 1)
							YIKES("/part expects at least one argument.");
						part_command(serv, il.args[0], il.rest()).send();
					} else std::cerr << "Unknown command: /" << il.command << std::endl;
				} catch (std::exception &err) {
					YIKES("Caught an exception (" << typeid(err).name() << "): " << err.what());
				}
			} else {
				
			}
		}
	}

	void client::add_listeners() {
		events::listen<message_event>([&](auto *ev) {
			if (!ev->msg->template is<numeric_message>() && !ev->msg->template is<ping_message>())
				pp->dbgout() << std::string(*(ev->msg)) << "\n";
		});
	}
}

int main(int argc, char **argv) {
	std::shared_ptr<irc> pp = irc::shared();
	pp->init();
	client instance(pp);
	instance.add_listeners();

	string hostname;

	hostname = 1 < argc? argv[1] : "localhost";
	server serv(pp, hostname);
	serv.start();
	serv.set_nick("pingpong");
	std::thread input(&client::input_worker, &instance, &serv);
	serv.server_thread->join();
	input.join();
}
