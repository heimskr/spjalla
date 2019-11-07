#include "spjalla/core/client.h"
#include "spjalla/lines/quit.h"

namespace spjalla::lines {
	std::string quit_line::render(ui::window *) {
		return notice + parent->get_ui().render.nick(name, false) + " has quit " + "["_d + message + "]"_d;
	}
}
