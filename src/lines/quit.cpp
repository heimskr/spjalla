#include "spjalla/core/client.h"
#include "spjalla/lines/quit.h"

namespace spjalla::lines {
	std::string quit_line::render(ui::window *) {
		return parent->get_ui().render("quit", {{"raw_who", name}, {"raw_reason", message}});
	}
}
