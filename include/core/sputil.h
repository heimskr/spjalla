#include <string>


namespace spjalla {
	namespace ui {
		class window;
	}

	struct util {
		/** Styles a window based on whether it's dead. */
		static std::string colorize_if_dead(const std::string &, const ui::window *);
	};
}
