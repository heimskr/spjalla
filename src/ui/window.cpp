#include "ui/window.h"

namespace spjalla::ui {
	void swap(window &left, window &right) {
		swap(static_cast<haunted::ui::textbox &>(left), static_cast<haunted::ui::textbox &>(right));
		std::swap(left.window_name, right.window_name);
	}
}
