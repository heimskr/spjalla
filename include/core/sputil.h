#include <string>


namespace spjalla {
	namespace ui {
		class window;
	}

	namespace util {
		/** Styles a window based on whether it's dead. */
		std::string colorize_if_dead(const std::string &, const ui::window *);
	}
}
