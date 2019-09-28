#include <filesystem>
#include <string>

namespace spjalla {
	namespace ui {
		class window;
	}

	namespace util {
		/** Styles a window based on whether it's dead. */
		std::string colorize_if_dead(const std::string &, const ui::window *);

		/** Returns a path to the user's home directory as a string ending with a slash. */
		std::string get_home_string();

		/** Returns a path to the user's home directory. */
		std::filesystem::path get_home();
	}
}
