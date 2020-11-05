#ifndef SPJALLA_PLUGINS_NOTIFICATIONSWIDGET_H_
#define SPJALLA_PLUGINS_NOTIFICATIONSWIDGET_H_

#include "spjalla/ui/StatusWidget.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	class NotificationsWidget: public Spjalla::UI::StatusWidget {
		protected:
			std::string _render(const UI::Window *, bool) const override;

		public:
			ansi::color highlightColor = ansi::color::yellow;
			bool highlightBold = true;

			using StatusWidget::StatusWidget;

			virtual ~NotificationsWidget();

			const char * getName() const override { return "Notifications"; }

			void windowFocused(UI::Window *window);
	};
}

#endif
