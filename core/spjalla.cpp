#include <iostream>
#include <string>

#include "core/irc.h"
#include "core/server.h"
#include "spjalla.h"
#include "lib/ansi.h"

using namespace pingpong;

int main(int argc, char **argv) {
	irc instance;

	string hostname;

	hostname = 1 < argc? argv[1] : "localhost";
	server serv(instance, hostname);
}
