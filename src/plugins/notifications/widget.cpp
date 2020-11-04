#include "spjalla/core/Client.h"
#include "spjalla/plugins/notifications/widget.h"

namespace Spjalla::Plugins {
	notifications_widget::~notifications_widget() = default;

	std::string notifications_widget::_render(const UI::Window *, bool) const {
		std::vector<std::string> indicators;

		std::deque<Haunted::UI::control *> controls = parent->get_window_controls();
		std::sort(controls.begin(), controls.end(), [](Haunted::UI::control *one, Haunted::UI::control *two) {
			return one && two && one->get_index() < two->get_index();
		});

		for (Haunted::UI::control *control: controls) {
			if (!control)
				continue;

			const ssize_t index = control->get_index();
			if (index < 0)
				continue;

			UI::Window *window = dynamic_cast<UI::Window *>(control);
			if (window == parent->getUI().get_active_window())
				continue;

			NotificationType type = window->highest_notification;
			if (type == NotificationType::none) {
				continue;
			}
			
			std::string index_str = std::to_string(index + 1);
			switch (type) {
				case NotificationType::Info:      indicators.push_back(ansi::dim(index_str)); break;
				case NotificationType::message:   indicators.push_back(index_str); break;
				case NotificationType::highlight: 
					indicators.push_back(ansi::wrap(highlight_bold? ansi::bold(index_str) : index_str,
						highlight_color));
					break;
				default: throw std::invalid_argument("Invalid notification type");
			}
		}

		return formicine::util::join(indicators.begin(), indicators.end(), ","_d);
	}

	void notifications_widget::window_focused(UI::Window *window) {
		window->unnotify();
	}
}
