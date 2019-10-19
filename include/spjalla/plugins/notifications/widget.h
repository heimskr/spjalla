#ifndef SPJALLA_PLUGINS_NOTIFICATIONS_WIDGET_H_
#define SPJALLA_PLUGINS_NOTIFICATIONS_WIDGET_H_

#include "spjalla/ui/status_widget.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	class notifications_widget: public spjalla::ui::status_widget {
		public:
			ansi::color highlight_color = ansi::color::yellow;
			bool highlight_bold = true;

			using status_widget::status_widget;

			virtual ~notifications_widget() {}

			std::string render(const ui::window *, bool) const;
			void window_focused(ui::window *window);
	};
}

#endif
