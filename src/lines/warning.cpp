#include "spjalla/core/client.h"
#include "spjalla/lines/warning.h"

namespace spjalla::lines {
	std::string warning_line::render(ui::window *) {
		return parent->get_ui().render("bang_warn") + " " + message;
	}
}
