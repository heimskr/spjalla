#ifndef SPJALLA_UI_STATUS_WIDGET_H_
#define SPJALLA_UI_STATUS_WIDGET_H_

#include <string>

#include "ui/window.h"

namespace spjalla {
	class client;
}

namespace spjalla::ui {
	/**
	 * Represents widgets that sit in the status bar.
	 */
	class status_widget {
		protected:
			client *parent;

		public:
			status_widget(client *parent_, int priority_ = 50): parent(parent_), priority(priority_) {}

			/** A lower priority indicates the widget should be ordered more towards the left of the status bar. */
			int priority;

			/** Surrounds a string (the rendered contents of the widget) with characters at each end ("[" and "]" by
			 *  default). */
			virtual std::string surround(const std::string &middle) const;

			/** Called whenever the widget should update its state. */
			virtual void update();

			/** Returns a string to be displayed in the status bar. */
			virtual std::string render() const = 0;

			/** Returns whether the widget should be displayed when the given window is the current window. */
			virtual bool visible_for(const window *, bool overlay_visible) const;
	};
}

#endif
