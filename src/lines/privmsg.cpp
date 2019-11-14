#include "pingpong/core/util.h"

#include "strender/strnode.h"

#include "spjalla/core/util.h"
#include "spjalla/lines/privmsg.h"
#include "spjalla/ui/renderer.h"

#include "lib/formicine/futil.h"

namespace spjalla::lines {
	notification_type privmsg_line::get_notification_type() const {
		if (util::is_highlight(message, self, direct_only) || where == self)
			return notification_type::highlight;
		return notification_type::message;
	}

	std::string privmsg_line::get_format_key() const {
		return is_action()? "action" : "privmsg";
	}
}
