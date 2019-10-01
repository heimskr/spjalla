#ifndef SPJALLA_EVENTS_WINDOW_CHANGED_H_
#define SPJALLA_EVENTS_WINDOW_CHANGED_H_

#include "pingpong/events/event.h"
#include "spjalla/ui/window.h"

namespace spjalla::events {
	/**
	 * Dispatched whenever the user switches from one window to another (including the overlay).
	 */
	struct window_changed_event: public pingpong::event {
		ui::window *from, *to;

		window_changed_event(ui::window *from_, ui::window *to_): from(from_), to(to_) {}
	};
}

#endif
