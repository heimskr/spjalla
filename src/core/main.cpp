#include <stdexcept>

#include <sys/types.h>
#include <unistd.h>

#include "pingpong/core/irc.h"
#include "core/client.h"
#include "core/util.h"

// #define MAIN_CATCH

namespace spjalla {
	void run() {
		haunted::dbgstream << "--------------------------------\n";
		haunted::dbgstream.clear().jump().flush();
		DBG("PID: " << getpid());

		std::shared_ptr<spjalla::client> instance = std::make_shared<spjalla::client>();

		instance->init();
		instance->join();
	}
}

int main(int, char **) {
#ifdef MAIN_CATCH
	try {
		spjalla::run();
	} catch (std::exception &exc) {
		DBG("Caught " << haunted::util::demangle_object(exc));
	}
#else
	spjalla::run();
#endif
}
