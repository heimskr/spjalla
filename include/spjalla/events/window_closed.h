#ifndef SPJALLA_EVENTS_WINDOW_CLOSED_H_
#define SPJALLA_EVENTS_WINDOW_CLOSED_H_

#include "pingpong/events/event.h"
#include "spjalla/ui/window.h"

namespace spjalla::events {
	/**
	 * 
	 */
	struct window_closed_event: public pingpong::event {
		ui::window *window;

		window_closed_event(ui::window *window_): window(window_) {}
	};
}

#endif
