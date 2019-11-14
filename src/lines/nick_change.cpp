#include "spjalla/core/client.h"
#include "spjalla/lines/nick_change.h"

namespace spjalla::lines {
	std::string nick_change_line::render(ui::window *) {
		return parent->get_ui().render("nick_change", {{"raw_new", new_name}, {"raw_old", old_name}});
	}
}
