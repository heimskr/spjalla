#include <stdexcept>

#include "core/irc.h"
#include "core/client.h"
#include "core/util.h"

// #define MAIN_CATCH

namespace spjalla {
	void run(int argc, char **argv) {
		std::shared_ptr<pingpong::irc> pp = std::make_shared<pingpong::irc>();
		std::shared_ptr<spjalla::client> cli = std::make_shared<spjalla::client>();

		haunted::dbgstream << "--------------------------------\n";
		haunted::dbgstream.clear().jump().flush();

		spjalla::client instance;
		instance.init();

		std::string hostname = 1 < argc? argv[1] : "localhost";
		// std::shared_ptr<server> sserv = std::make_shared<server>(pp, hostname);
		// server_ptr serv = sserv.get();
		// serv->start();
		// serv->set_nick("pingpong");
		// instance += serv;
	}
}

int main(int argc, char **argv) {
#ifdef MAIN_CATCH
	try {
		spjalla::run(argc, argv);
	} catch (std::exception &exc) {
		DBG("Caught " << haunted::util::demangle_object(exc));
	}
#else
	spjalla::run(argc, argv);
#endif
}
