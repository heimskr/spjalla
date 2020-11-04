#include <filesystem>
#include <stdexcept>

#include <sys/types.h>
#include <unistd.h>

#include "spjalla/core/Options.h"

#include "pingpong/core/IRC.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"

#include "spjalla/lines/Warning.h"

// #define MAIN_CATCH

namespace Spjalla {
	std::string getPluginDirectory(int argc, char **argv) {
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
		Haunted::dbgstream << "--------------------------------\n";
		Haunted::dbgstream.clear().jump().flush();
		DBG(ansi::style::bold << "    " << getpid());

		std::shared_ptr<Spjalla::Client> instance = std::make_shared<Spjalla::Client>();
		instance->getTerminal().mouse(Haunted::MouseMode::Motion);
		instance->getIRC().version = "Spjalla " SPJALLA_VERSION_NUMBER " with pingpong " PINGPONG_VERSION_NUMBER;

		const std::string plugin_dir = getPluginDirectory(argc, argv);
		std::string warning;
		try {
			instance->loadPlugins(plugin_dir);
			DBG("Loaded plugins.");
		} catch (const std::filesystem::filesystem_error &err) {
			warning = "Couldn't load plugins from " + ansi::bold(plugin_dir) + ": " + err.code().message();
		}

		instance->preinitPlugins();
		instance->init();
		if (!warning.empty())
			instance->log(Lines::WarningLine(instance.get(), warning));
		instance->postinitPlugins();
		instance->postinit();
		instance->join();
	}
}

int main(int argc, char **argv) {
#ifdef MAIN_CATCH
	try {
		spjalla::run(argc, argv);
	} catch (std::exception &exc) {
		DBG("Caught " << Haunted::util::demangle_object(exc));
	}
#else
	Spjalla::run(argc, argv);
#endif
}
