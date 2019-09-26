#include "ui/status_widget.h"

#include "formicine/ansi.h"

namespace spjalla::ui {
	std::string status_widget::surround(const std::string &middle) const {
		return "["_d + middle + "]"_d;
	}

	void status_widget::update() {}

	bool status_widget::visible_for(const window *, bool) const {
		return true;
	}
}
