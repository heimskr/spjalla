#ifndef SPJALLA_LINES_LINES_H_
#define SPJALLA_LINES_LINES_H_

#include <string>

#include "pingpong/core/util.h"
#include "haunted/ui/textbox.h"
#include "spjalla/core/notifications.h"
#include "lib/formicine/ansi.h"

namespace spjalla::lines {
	extern std::string notice;
	extern std::string red_notice;

	/** Renders a UNIX timestamp as an hours-minutes-seconds set. */
	std::string render_time(long seconds);

	struct line: haunted::ui::textline {
		using haunted::ui::textline::textline;

		virtual notification_type get_notification_type() const { return notification_type::none; }
	};
}

#endif
