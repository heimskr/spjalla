#ifndef SPJALLA_LINES_NOTICE_H_
#define SPJALLA_LINES_NOTICE_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/notice.h"
#include "pingpong/events/notice.h"

#include "spjalla/lines/message.h"

namespace spjalla::lines {
	class notice_line: public message_line {
		private:
			bool always_highlight = false;

		public:
			using message_line::message_line;

			notice_line(client *parent_, const pingpong::notice_command &cmd, bool direct_only_ = false):
				notice_line(parent_, cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time, direct_only_) {}

			notice_line(client *parent_, const pingpong::notice_event &ev, bool direct_only_ = false,
			            bool always_highlight_ = false):
				notice_line(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {
				always_highlight = always_highlight_;
			}

			notification_type get_notification_type() const override;
			std::string get_format_key() const override;
	};
}

#endif
