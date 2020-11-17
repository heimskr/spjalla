#ifndef SPJALLA_UI_BASICWINDOW_H_
#define SPJALLA_UI_BASICWINDOW_H_

#include "spjalla/ui/Window.h"

namespace Spjalla::UI {
	class BasicWindow: public Window {
		public:
			using Window::Window;

			bool shouldShowTimes() const override {
				return false;
			}
	};
}

#endif