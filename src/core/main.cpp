#include <filesystem>
#include <stdexcept>

#include <sys/types.h>
#include <unistd.h>

#include "pingpong/core/irc.h"

#include "spjalla/core/client.h"
#include "spjalla/core/util.h"

#include "spjalla/lines/warning.h"

// #define MAIN_CATCH

namespace spjalla {
	std::string get_plugin_dir(int argc, char **argv) {
		if (2 <= argc) {
			for (int i = 1; i < argc - 1; ++i) {
				std::string arg {argv[i]};
				if (arg == "-p" || arg == "--plugins")
					return argv[i + 1];
			}
		}

		return "plugins";
	}

	void run(int argc, char **argv) {
		haunted::dbgstream << "--------------------------------\n";
		haunted::dbgstream.clear().jump().flush();
		DBG("PID: " << getpid());

		std::shared_ptr<spjalla::client> instance = std::make_shared<spjalla::client>();

		const std::string plugin_dir = get_plugin_dir(argc, argv);
		try {
			instance->load_plugins(plugin_dir);
			DBG("Loaded plugins.");
		} catch (const std::filesystem::filesystem_error &err) {
			instance->log(lines::warning_line("Couldn't load plugins from " + ansi::bold(plugin_dir) + ": " +
				err.code().message()));
		}

		instance->init();
		instance->init_plugins();
		instance->join();
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
