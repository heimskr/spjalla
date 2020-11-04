#include "spjalla/ui/StatusWidget.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::UI {
	std::string StatusWidget::surround(const std::string &middle) const {
		return "["_d + middle + "]"_d;
	}

	void StatusWidget::update() {}

	std::string StatusWidget::render(const Window *win, bool overlay_visible) const {
		return parent? _render(win, overlay_visible) : "";
	}

	bool StatusWidget::visibleFor(const Window *, bool) const {
		return true;
	}
}
