#include "spjalla/core/client.h"
#include "spjalla/lines/error.h"

namespace spjalla::lines {
	std::string error_line::render(ui::window *) {
		return parent->get_ui().render("bang_bad") + " " + message;
	}
}
