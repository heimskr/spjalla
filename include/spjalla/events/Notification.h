#ifndef SPJALLA_EVENTS_NOTIFICATION_H_
#define SPJALLA_EVENTS_NOTIFICATION_H_

#include "pingpong/events/Event.h"
#include "spjalla/ui/Window.h"
#include "spjalla/lines/Line.h"

namespace Spjalla::Events {
	/**
	 * Dispatched whenever a new notification is received.
	 */
	struct NotificationEvent: public PingPong::Event {
		UI::Window *window;
		std::shared_ptr<Lines::Line> line;
		NotificationType type;

		NotificationEvent(UI::Window *window_, std::shared_ptr<Lines::Line> line_, NotificationType type_):
			window(window_), line(line_), type(type_) {}
	};

	/**
	 * Dispatched whenever a window's highest notification type changes.
	 */
	struct WindowNotificationEvent: public NotificationEvent {
		NotificationType old_type;
		WindowNotificationEvent(UI::Window *window_, std::shared_ptr<Lines::Line> line_, NotificationType new_type_,
		NotificationType old_type_):
			NotificationEvent(window_, line_, new_type_), old_type(old_type_) {}
	};
}


#endif
