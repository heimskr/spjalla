#include "spjalla/core/client.h"
#include "spjalla/plugins/notifications/widget.h"

namespace spjalla::plugins {
	std::string notifications_widget::render(const ui::window *, bool) const {
		std::vector<std::string> indicators;

		std::deque<haunted::ui::control *> controls = parent->get_window_controls();
		std::sort(controls.begin(), controls.end(), [](haunted::ui::control *one, haunted::ui::control *two) {
			return one && two && one->get_index() < two->get_index();
		});

		for (haunted::ui::control *control: controls) {
			if (!control)
				continue;

			const ssize_t index = control->get_index();
			if (index < 0)
				continue;

			ui::window *window = dynamic_cast<ui::window *>(control);
			if (window == parent->get_ui().get_active_window())
				continue;

			notification_type type = window->highest_notification;
			if (type == notification_type::none) {
				continue;
			}
			
			std::string index_str = std::to_string(index + 1);
			switch (type) {
				case notification_type::info:      indicators.push_back(ansi::dim(index_str)); break;
				case notification_type::message:   indicators.push_back(index_str); break;
				case notification_type::highlight: 
					indicators.push_back(ansi::wrap(highlight_bold? ansi::bold(index_str) : index_str,
						highlight_color));
					break;
				default: throw std::invalid_argument("Invalid notification type");
			}
		}

		return formicine::util::join(indicators.begin(), indicators.end(), ","_d);
	}

	void notifications_widget::window_focused(ui::window *window) {
		window->unnotify();
	}
}
