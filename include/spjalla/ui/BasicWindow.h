#ifndef SPJALLA_UI_BASIC_WINDOW_H_
#define SPJALLA_UI_BASIC_WINDOW_H_

#include "spjalla/ui/window.h"

namespace Spjalla::UI {
	class basic_window: public window {
		public:
			using window::window;

			bool show_times() const override {
				return false;
			}
	};
}

#endif