#include "spjalla/core/client.h"
#include "spjalla/lines/part.h"

namespace spjalla::lines {
	std::string part_line::render(ui::window *) {
		return parent->get_ui().render("part", {
			{"raw_who", user->name}, {"raw_channel", chan->name}, {"raw_reason", reason}
		});
	}
}
