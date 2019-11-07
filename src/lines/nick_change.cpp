#include "spjalla/core/client.h"
#include "spjalla/lines/nick_change.h"

namespace spjalla::lines {
	std::string nick_change_line::render(ui::window *) {
		return notice + parent->get_ui().render.nick(old_name, false) + " is now known as "
			+ parent->get_ui().render.nick(new_name, true);
	}
}
