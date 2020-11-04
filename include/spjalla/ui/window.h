#ifndef SPJALLA_UI_WINDOW_H_
#define SPJALLA_UI_WINDOW_H_

#include <functional>
#include <optional>

#include "haunted/core/Defs.h"
#include "haunted/ui/Container.h"
#include "haunted/ui/Textbox.h"
#include "pingpong/core/Defs.h"
#include "spjalla/core/Notifications.h"
#include "spjalla/lines/Line.h"

#include "lib/formicine/performance.h"

namespace Spjalla::UI {
	enum class WindowType {
		Status,  // The single status window where miscellaneous messages go.
		Overlay, // The window that replaces the sidebar (RIP) and can be summoned with a keypress.
		Channel, // A window containing the conversation within an IRC channel.
		User,    // A window for a private conversation with another user.
		Other    // A window used for any purpose not covered by the other window types and when the type is unknown.
	};

	/**
	 * Represents a type of textbox for use within a swapbox. It has a window name separate from the control ID, in
	 * addition to other data and metadata.
	 */
	class Window: public Haunted::UI::Textbox {
		private:
			void addLine(std::shared_ptr<Haunted::UI::TextLine>);

		public:
			std::string windowName;
			WindowType type = WindowType::Other;
			PingPong::Server *server = nullptr;
			std::shared_ptr<PingPong::Channel> channel;
			std::shared_ptr<PingPong::User>    user;
			NotificationType highestNotification = NotificationType::None;

			/** Whether whatever the window is for is dead—e.g., a channel you've been kicked from. */
			bool dead = false;

			Window() = delete;
			Window(const Window &) = delete;
			Window & operator=(const Window &) = delete;

			/** Constructs a window with a parent, a position and initial contents. */
			Window(Haunted::UI::Container *parent_, Haunted::Position position_,
			const std::vector<std::string> &contents_, const std::string &window_name):
				Textbox(parent_, position_, contents_), windowName(window_name) {}

			/** Constructs a window with a parent and position and empty contents. */
			Window(Haunted::UI::Container *parent_, Haunted::Position position_, const std::string &window_name):
				Window(parent_, position_, {}, window_name) {}

			/** Constructs a window with a parent, initial contents and a default position. */
			Window(Haunted::UI::Container *parent_, const std::vector<std::string> &contents_,
			const std::string &window_name):
				Textbox(parent_, contents_), windowName(window_name) {}

			/** Constructs a window with a parent, a default position and empty contents. */
			Window(Haunted::UI::Container *parent_, const std::string &window_name):
				Window(parent_, std::vector<std::string> {}, window_name) {}

			/** Constructs a window with no parent and no contents. */
			Window(const std::string &window_name):
				Window(nullptr, std::vector<std::string>(), window_name) {}

			/** Whether lines rendered in the window should begin with a timestamp. */
			virtual bool show_times() const;

			template <typename T, typename std::enable_if_t<std::is_base_of_v<Lines::Line, T>> * = nullptr>
			Window & operator+=(const T &line) {
				auto w = formicine::perf.watch("template <line> window::operator+=");
				return *this += std::make_shared<T>(line);
			}

			template <typename T, typename std::enable_if_t<std::is_base_of_v<Haunted::UI::TextLine, T>> * = nullptr,
			                      typename std::enable_if_t<!std::is_base_of_v<Lines::Line, T>> * = nullptr>
			Window & operator+=(const T &line) {
				auto w = formicine::perf.watch("template <!line> window::operator+=");
				return *this += std::make_shared<T>(line);
			}

			template <typename T, typename std::enable_if_t<std::is_base_of_v<Lines::Line, T>> * = nullptr,
			                      typename std::enable_if_t<std::is_constructible_v<Lines::Line, T>> * = nullptr>
			Window & operator+=(std::shared_ptr<T> line) {
				auto w = formicine::perf.watch("window::operator+=(shared_ptr<Line>)");

				if (line->box && line->box != this)
					line = std::make_shared<T>(*line);

				line->box = this;
				addLine(line);

				notify(line, line->getNotificationType());
				return *this;
			}

			template <typename T, typename std::enable_if_t<std::is_base_of_v<Lines::Line, T>> * = nullptr,
			                      typename std::enable_if_t<!std::is_constructible_v<T>> * = nullptr>
			Window & operator+=(std::shared_ptr<T> line) {
				auto w = formicine::perf.watch("window::operator+=(shared_ptr<Line>)");

				line->box = this;
				addLine(line);

				notify(line, line->getNotificationType());
				return *this;
			}

			template <typename T, typename std::enable_if_t<std::is_base_of_v<Haunted::UI::TextLine, T>> * = nullptr,
			                      typename std::enable_if_t<!std::is_base_of_v<Lines::Line, T>> * = nullptr>
			Window & operator+=(std::shared_ptr<T> line) {
				auto w = formicine::perf.watch("window::operator+=(shared_ptr<TextLine>)");
				addLine(line);
				return *this;
			}

			Window & operator+=(const std::string &);

			template <typename T, typename... Args>
			void add(Args && ...args) {
				*this += std::make_shared<T>(std::forward<Args>(args)...);
			}

			bool isStatus() const;
			bool isOverlay() const;
			bool isChannel() const;
			bool isUser() const;
			bool isOther() const;

			bool isDead() const;

			void kill();
			void resurrect();

			void notify(std::shared_ptr<Lines::Line>, NotificationType);
			void notify(std::shared_ptr<Lines::Line>);
			void unnotify();

			/** Removes rows for which a given function returns true. */
			void removeRows(std::function<bool(const Haunted::UI::TextLine *)>);

			friend void swap(Window &left, Window &right);
	};
}

#endif
