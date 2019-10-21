#include "spjalla/lines/error.h"

namespace spjalla::lines {
	std::string error_line::render(ui::window *) {
		return lines::red_notice + message;
	}
}
