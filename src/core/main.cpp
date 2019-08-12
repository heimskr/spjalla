#include "core/irc.h"
#include "core/client.h"

int main(int argc, char **argv) {
	std::shared_ptr<pingpong::irc> pp = std::make_shared<pingpong::irc>();
	std::shared_ptr<spjalla::client> cli = std::make_shared<spjalla::client>();


	haunted::dbgstream.clear().jump() << "\n\n\n\n\n\n";


	spjalla::client instance;
	instance.init();


	std::string hostname = 1 < argc? argv[1] : "localhost";
	// std::shared_ptr<server> sserv = std::make_shared<server>(pp, hostname);
	// server_ptr serv = sserv.get();
	// serv->start();
	// serv->set_nick("pingpong");
	// instance += serv;
	instance.start_input();
}
