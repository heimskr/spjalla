#ifndef SPJALLA_EVENTS_NOTIFICATION_H_
#define SPJALLA_EVENTS_NOTIFICATION_H_

#include "pingpong/events/event.h"
#include "spjalla/ui/window.h"
#include "spjalla/lines/line.h"

namespace spjalla::events {
	/**
	 * Dispatched whenever a new notification is received.
	 */
	struct notification_event: public pingpong::event {
		ui::window *window;
		const lines::line *line;
		notification_type type;

		notification_event(ui::window *window_, const lines::line *line_, notification_type type_):
		window(window_), line(line_), type(type_) {}
	};

	/**
	 * Dispatched whenever a window's highest notification type changes.
	 */
	struct window_notification_event: public notification_event {
		notification_type old_type;
		window_notification_event(ui::window *window_, const lines::line *line_, notification_type new_type_,
		notification_type old_type_):
			notification_event(window_, line_, new_type_), old_type(old_type_) {}
	};
}


#endif
