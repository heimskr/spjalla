#include "spjalla/lines/alias.h"

namespace spjalla::lines {
	std::string alias_line::render(ui::window *) {
		return key + " = "_d + ansi::bold(expansion);
	}
}
