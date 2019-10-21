#include "spjalla/lines/warning.h"

namespace spjalla::lines {
	std::string warning_line::render(ui::window *) {
		return lines::yellow_notice + message;
	}
}
