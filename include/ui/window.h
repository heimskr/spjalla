#ifndef SPJALLA_UI_WINDOW_H_
#define SPJALLA_UI_WINDOW_H_

#include "lib/haunted/core/defs.h"
#include "lib/haunted/ui/container.h"
#include "lib/haunted/ui/textbox.h"

namespace spjalla::ui {
	class window: public haunted::ui::textbox {
		public:
			std::string window_name;

			window() = delete;
			window(const window &) = delete;
			window & operator=(const window &) = delete;

			/** Constructs a window with a parent, a position and initial contents. */
			window(haunted::ui::container *parent_, haunted::position pos_, const std::vector<std::string> &contents_,
				const std::string &window_name_): textbox(parent_, pos_, contents_), window_name(window_name_) {}

			/** Constructs a window with a parent and position and empty contents. */
			window(haunted::ui::container *parent_, haunted::position pos_, const std::string &window_name_):
				window(parent_, pos_, {}, window_name_) {}

			/** Constructs a window with a parent, initial contents and a default position. */
			window(haunted::ui::container *parent_, const std::vector<std::string> &contents_,
				const std::string &window_name_): textbox(parent_, contents_), window_name(window_name_) {}

			/** Constructs a window with a parent, a default position and empty contents. */
			window(haunted::ui::container *parent_, const std::string &window_name_):
				window(parent_, std::vector<std::string> {}, window_name_) {}

			/** Constructs a window with no parent and no contents. */
			window(const std::string &window_name_): window(nullptr, std::vector<std::string> {}, window_name_) {}

			friend void swap(window &left, window &right);
	};
}

#endif
