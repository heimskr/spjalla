#ifndef SPJALLA_LINES_NOTICE_H_
#define SPJALLA_LINES_NOTICE_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

// #include "pingpong/commands/notice.h"
#include "pingpong/events/notice.h"

#include "spjalla/lines/message.h"

namespace spjalla::lines {
	struct notice_format { static constexpr const char *message = "^d-^D%s^d-^0 %m", *action = "^d-^b*^B %s^d-^0 %m"; };

	class notice_line: public message_line<notice_format> {
		private:
			bool always_highlight = false;

		public:
			using message_line::message_line;

			notice_line(client *parent_, const pingpong::notice_event &ev, bool direct_only_ = false,
			bool always_highlight_ = false):
			notice_line(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {
				always_highlight = always_highlight_;
			}

			notification_type get_notification_type() const override;

			void postprocess(std::string &) const override;

			static std::string to_string(const pingpong::notice_event &, bool with_time = true);
	};
}

#endif
