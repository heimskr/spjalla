#include "spjalla/core/Client.h"
#include "spjalla/plugins/notifications/Widget.h"

namespace Spjalla::Plugins {
	NotificationsWidget::~NotificationsWidget() = default;

	std::string NotificationsWidget::_render(const UI::Window *, bool) const {
		std::vector<std::string> indicators;

		std::deque<Haunted::UI::Control *> controls = parent->getWindowControls();
		std::sort(controls.begin(), controls.end(), [](Haunted::UI::Control *one, Haunted::UI::Control *two) {
			return one && two && one->getIndex() < two->getIndex();
		});

		for (Haunted::UI::Control *control: controls) {
			if (!control)
				continue;

			const ssize_t index = control->getIndex();
			if (index < 0)
				continue;

			UI::Window *window = dynamic_cast<UI::Window *>(control);
			if (window == parent->getUI().getActiveWindow())
				continue;

			NotificationType type = window->highestNotification;
			if (type == NotificationType::None) {
				continue;
			}
			
			std::string index_str = std::to_string(index + 1);
			switch (type) {
				case NotificationType::Info:    indicators.push_back(ansi::dim(index_str)); break;
				case NotificationType::Message: indicators.push_back(index_str); break;
				case NotificationType::Highlight: 
					indicators.push_back(ansi::wrap(highlightBold? ansi::bold(index_str) : index_str,
						highlightColor));
					break;
				default: throw std::invalid_argument("Invalid notification type");
			}
		}

		return formicine::util::join(indicators.begin(), indicators.end(), ","_d);
	}

	void NotificationsWidget::windowFocused(UI::Window *window) {
		window->unnotify();
	}
}
