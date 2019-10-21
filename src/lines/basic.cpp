#include "spjalla/lines/basic.h"

namespace spjalla::lines {
	std::string basic_line::render(ui::window *) {
		return text;
	}
}
