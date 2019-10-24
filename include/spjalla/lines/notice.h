#ifndef SPJALLA_LINES_NOTICE_H_
#define SPJALLA_LINES_NOTICE_H_

#include "pingpong/core/defs.h"
#include "pingpong/core/hats.h"
#include "pingpong/core/local.h"

#include "pingpong/commands/notice.h"
#include "pingpong/events/notice.h"

#include "spjalla/lines/message.h"

namespace spjalla::lines {
	struct notice_format {
		static constexpr const char
			*message = "^d-^D#s^d-^0 #m",
			*action  = "^d-^b*^B #s^d-^0 #m",
			*ctcp    = "^d[^D#s^d]^0 #m";
		template <typename T>
		static void postprocess(T *line, std::string &str) {
			client *parent = line->parent;
			if (parent == nullptr)
				return;
			ansi::color foreground = ansi::get_color(parent->configs.get("appearance", "notice_foreground").string_());
			if (foreground != ansi::color::normal)
				str.insert(0, ansi::get_fg(foreground));
		}
	};

	class notice_line: public message_line<notice_format> {
		private:
			bool always_highlight = false;

		public:
			using message_line::message_line;

			notice_line(client *parent_, const pingpong::notice_command &cmd, bool direct_only_ = false):
				notice_line(parent_, cmd.serv->get_self(), cmd.where, cmd.message, cmd.sent_time, direct_only_) {}

			notice_line(client *parent_, const pingpong::notice_event &ev, bool direct_only_ = false,
			bool always_highlight_ = false):
			message_line(parent_, ev.speaker, ev.where, ev.content, ev.stamp, direct_only_) {
				always_highlight = always_highlight_;
			}

			notification_type get_notification_type() const override;

			static std::string to_string(const pingpong::notice_event &);
	};
}

#endif
