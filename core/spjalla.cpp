#include <iostream>
#include <string>
#include <thread>

#include "core/irc.h"
#include "core/server.h"
#include "lib/ansi.h"

#include "core/spjalla.h"
#include "core/input_line.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	void input_worker() {
		std::string in;
		while (std::getline(std::cin, in)) {
			std::cout << "(" << in << ")" << std::endl;
			input_line il = input_line(in);
			std::cout << std::string(il) << std::endl;
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
