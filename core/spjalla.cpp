#include <iostream>
#include <string>
#include <thread>

#include "core/irc.h"
#include "core/server.h"
#include "lib/ansi.h"
#include "spjalla.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	void input_worker() {
		std::string in;
		while (std::cin >> in) {
			
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
	std::thread input(&input_worker);
	serv.server_thread->join();
	input.join();
}
