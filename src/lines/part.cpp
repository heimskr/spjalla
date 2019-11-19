#include "spjalla/core/client.h"
#include "spjalla/lines/part.h"

namespace spjalla::lines {
	std::string part_line::render(ui::window *) {
		return parent->get_ui().render("part", {
			{"raw_who", name}, {"raw_channel", chan_name}, {"raw_reason", reason}
		});
	}
}
