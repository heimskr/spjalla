#ifndef SPJALLA_EVENTS_WINDOW_CHANGED_H_
#define SPJALLA_EVENTS_WINDOW_CHANGED_H_

#include "pingpong/events/Event.h"
#include "spjalla/ui/Window.h"

namespace Spjalla::Events {
	/**
	 * Dispatched whenever the user switches from one window to another (including the overlay).
	 */
	struct WindowChangedEvent: public PingPong::Event {
		UI::Window *from, *to;

		WindowChangedEvent(UI::Window *from_, UI::Window *to_): from(from_), to(to_) {}
	};
}

#endif
