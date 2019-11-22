#include "spjalla/core/client.h"
#include "spjalla/lines/success.h"

namespace spjalla::lines {
	std::string success_line::render(ui::window *) {
		return parent->get_ui().render("bang_good") + " " + message;
	}
}
