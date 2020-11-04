#ifndef SPJALLA_LINES_NOTICE_H_
#define SPJALLA_LINES_NOTICE_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/Hats.h"
#include "pingpong/core/Local.h"

#include "pingpong/commands/Notice.h"
#include "pingpong/events/Notice.h"

#include "spjalla/lines/Message.h"

namespace Spjalla::Lines {
	class NoticeLine: public MessageLine {
		private:
			bool alwaysHighlight = false;

		public:
			using MessageLine::MessageLine;

			NoticeLine(Client *parent_, const PingPong::NoticeCommand &cmd, bool direct_only_ = false):
				NoticeLine(parent_, cmd.server->getSelf(), cmd.where, cmd.message, cmd.sentTime, direct_only_) {}

			NoticeLine(Client *parent_, const PingPong::NoticeEvent &ev, bool direct_only_ = false,
			bool always_highlight = false):
			NoticeLine(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {
				alwaysHighlight = always_highlight;
			}

			NotificationType getNotificationType() const override;
			std::string getFormatKey() const override;
	};
}

#endif
