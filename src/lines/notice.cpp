#include "pingpong/core/Util.h"
#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"
#include "spjalla/lines/Notice.h"
#include "lib/formicine/futil.h"

namespace Spjalla::Lines {
	NotificationType NoticeLine::getNotificationType() const {
		if (Util::isHighlight(message, self, directOnly) || where == self || alwaysHighlight)
			return NotificationType::Highlight;
		return NotificationType::Message;
	}

	std::string NoticeLine::getFormatKey() const {
		return "notice";
	}
}
