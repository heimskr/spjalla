#include "pingpong/core/util.h"
#include "spjalla/core/client.h"
#include "spjalla/core/util.h"
#include "spjalla/lines/notice.h"
#include "lib/formicine/futil.h"

namespace spjalla::lines {
	notification_type notice_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self || always_highlight)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string notice_line::render(ui::window *) {
		return parent->get_ui().render("notice", {
			{"raw_nick", name}, {"hats", hat_str()}, {"raw_message", pingpong::util::irc2ansi(message)}
		});
	}
}
