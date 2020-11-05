#include "spjalla/core/Client.h"
#include "spjalla/lines/Kick.h"

namespace Spjalla::Lines {
	std::string KickLine::render(UI::Window *) {
		return parent->getUI().renderer(isSelf? "kick_self" : "kick", {
			{"raw_who", who}, {"raw_whom", whom}, {"raw_channel", channel->name}, {"raw_reason", reason}
		});
	}

	NotificationType KickLine::getNotificationType() const {
		return isSelf? NotificationType::Highlight : NotificationType::Info;
	}
}
