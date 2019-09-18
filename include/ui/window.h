#ifndef SPJALLA_UI_WINDOW_H_
#define SPJALLA_UI_WINDOW_H_

#include <optional>

#include "lib/haunted/core/hdefs.h"
#include "lib/haunted/ui/container.h"
#include "lib/haunted/ui/textbox.h"
#include "lib/pingpong/core/ppdefs.h"

namespace spjalla::ui {

	enum class window_type {
		status,  // The single status window where miscellaneous messages go.
		overlay, // The window that replaces the sidebar (RIP) and can be summoned with a keypress.
		channel, // A window containing the conversation within an IRC channel.
		user,    // A window for a private conversation with another user.
		other    // A window used for any purpose not covered by the other window types and when the type is unknown.
	};

	/**
	 * Represents metadata useful for windows.
	 */ 
	struct window_meta {
		window_type type;

		pingpong::server *serv = nullptr;
		std::shared_ptr<pingpong::channel> chan;
		std::shared_ptr<pingpong::user>    user;

		/** Whether whatever the window is for is dead—e.g., a channel you've been kicked from. */
		bool dead = false;
		
		void *other = nullptr;

		window_meta(window_type type_): type(type_) {}
	};

	/**
	 * Represents a type of textbox for use within a swapbox. It has a window name separate from the control ID, in
	 * addition to an optional data object.
	 */
	class window: public haunted::ui::textbox {
		public:
			std::string window_name;
			window_meta data {window_type::other};

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

			bool is_status() const;
			bool is_overlay() const;
			bool is_channel() const;
			bool is_user() const;
			bool is_other() const;

			bool is_dead() const;

			friend void swap(window &left, window &right);
	};
}

#endif
