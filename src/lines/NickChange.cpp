#include "spjalla/core/Client.h"
#include "spjalla/lines/NickChange.h"

namespace Spjalla::Lines {
	std::string NickChangeLine::render(UI::Window *) {
		return parent->getUI().renderer("nick_change", {{"raw_new", newName}, {"raw_old", oldName}});
	}
}
