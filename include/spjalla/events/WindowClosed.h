#ifndef SPJALLA_EVENTS_WINDOW_CLOSED_H_
#define SPJALLA_EVENTS_WINDOW_CLOSED_H_

#include "pingpong/events/Event.h"
#include "spjalla/ui/Window.h"

namespace Spjalla::Events {
	struct WindowClosedEvent: public PingPong::Event {
		UI::Window *window;

		WindowClosedEvent(UI::Window *window_): window(window_) {}
	};
}

#endif
