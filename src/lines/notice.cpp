#include "pingpong/core/util.h"
#include "spjalla/core/util.h"
#include "spjalla/lines/notice.h"
#include "lib/formicine/futil.h"

namespace spjalla::lines {
	notification_type notice_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string notice_line::to_string(const pingpong::notice_event &ev, bool with_time) {
		notice_line line {ev};
		return ansi::strip(line.process(ev.content, with_time));
	}
}
