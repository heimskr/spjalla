#ifndef SPJALLA_UI_STATUS_WIDGET_H_
#define SPJALLA_UI_STATUS_WIDGET_H_

#include <string>

#include "spjalla/ui/Window.h"

namespace Spjalla {
	class Client;
}

namespace Spjalla::UI {
	/**
	 * Represents widgets that sit in the status bar.
	 */
	class StatusWidget {
		protected:
			Client *parent;

			/** Implementation for render(). */
			virtual std::string _render(const Window *, bool overlay_visible) const = 0;

		public:
			/** A lower priority indicates the widget should be ordered more towards the left of the status bar. */
			int priority;

			StatusWidget(Client *parent_, int priority_ = 50): parent(parent_), priority(priority_) {}

			virtual const char * getName() const { return "?"; }

			/** Surrounds a string (the rendered contents of the widget) with characters at each end ("[" and "]" by
			 *  default). */
			virtual std::string surround(const std::string &middle) const;

			/** Called whenever the widget should update its state. */
			virtual void update();

			/** Returns a string to be displayed in the status bar. */
			std::string render(const Window *, bool overlay_visible) const;

			/** Returns whether the widget should be displayed when the given window is the current window. */
			virtual bool visibleFor(const Window *, bool overlay_visible) const;
	};
}

#endif
