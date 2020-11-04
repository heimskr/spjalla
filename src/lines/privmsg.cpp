#include "pingpong/core/Util.h"

#include "strender/StrNode.h"

#include "spjalla/core/Util.h"
#include "spjalla/lines/Privmsg.h"
#include "spjalla/ui/Renderer.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Lines {
	NotificationType PrivmsgLine::getNotificationType() const {
		if (Util::isHighlight(message, self, directOnly) || where == self)
			return NotificationType::Highlight;
		return NotificationType::Message;
	}

	std::string PrivmsgLine::getFormatKey() const {
		return isAction()? "action" : "privmsg";
	}
}
