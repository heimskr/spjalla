#ifndef SPJALLA_UI_WINDOW_H_
#define SPJALLA_UI_WINDOW_H_

#include <functional>
#include <optional>

#include "haunted/core/defs.h"
#include "haunted/ui/container.h"
#include "haunted/ui/textbox.h"
#include "pingpong/core/defs.h"
#include "spjalla/core/notifications.h"
#include "spjalla/lines/line.h"

namespace spjalla::ui {
	enum class window_type {
		status,  // The single status window where miscellaneous messages go.
		overlay, // The window that replaces the sidebar (RIP) and can be summoned with a keypress.
		channel, // A window containing the conversation within an IRC channel.
		user,    // A window for a private conversation with another user.
		other    // A window used for any purpose not covered by the other window types and when the type is unknown.
	};

	/**
	 * Represents a type of textbox for use within a swapbox. It has a window name separate from the control ID, in
	 * addition to other data and metadata.
	 */
	class window: public haunted::ui::textbox {
		public:
			std::string window_name;
			window_type type = window_type::other;
			pingpong::server *serv = nullptr;
			std::shared_ptr<pingpong::channel> chan;
			std::shared_ptr<pingpong::user>    user;
			notification_type highest_notification = notification_type::none;

			/** Whether whatever the window is for is dead—e.g., a channel you've been kicked from. */
			bool dead = false;

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

			template <typename T, typename std::enable_if<std::is_base_of<lines::line, T>::value>::type * = nullptr>
			textbox & operator+=(const T &line) {
				std::unique_ptr<T> line_copy = std::make_unique<T>(line);
				const bool did_scroll = do_scroll(line.num_rows(pos.width));
				lines.push_back(std::move(line_copy));

				if (!did_scroll)
					draw_new_line(*lines.back(), true);

				notify(line, line.get_notification_type());
				return *this;
			}

			template <typename T,
				typename std::enable_if<std::is_base_of<haunted::ui::textline, T>::value>::type * = nullptr,
				typename std::enable_if<!std::is_base_of<lines::line, T>::value>::type * = nullptr>
			textbox & operator+=(const T &line) {
				std::unique_ptr<T> line_copy = std::make_unique<T>(line);
				lines.push_back(std::move(line_copy));
				if (!do_scroll(line.num_rows(pos.width)))
					draw_new_line(*lines.back(), true);
				return *this;
			}

			bool is_status() const;
			bool is_overlay() const;
			bool is_channel() const;
			bool is_user() const;
			bool is_other() const;

			bool is_dead() const;

			void kill();
			void resurrect();

			void notify(const lines::line &, notification_type);
			void notify(const lines::line &);
			void unnotify();

			/** Removes rows for which a given function returns true. */
			void remove_rows(std::function<bool(const haunted::ui::textline *)>);

			friend void swap(window &left, window &right);
	};
}

#endif
