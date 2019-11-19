#include "spjalla/ui/status_widget.h"

#include "lib/formicine/ansi.h"

namespace spjalla::ui {
	std::string status_widget::surround(const std::string &middle) const {
		return "["_d + middle + "]"_d;
	}

	void status_widget::update() {}

	std::string status_widget::render(const window *win, bool overlay_visible) const {
		return parent? _render(win, overlay_visible) : "";
	}

	bool status_widget::visible_for(const window *, bool) const {
		return true;
	}
}
