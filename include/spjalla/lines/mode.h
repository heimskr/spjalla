#ifndef SPJALLA_LINES_MODE_H_
#define SPJALLA_LINES_MODE_H_

#include <unordered_map>

#include "pingpong/events/mode.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class mode_line: public line {
		private:
			/** Maps a mode change (e.g., "+o", "-b") to a verb that indicates what the mode change means. */
			static std::unordered_map<std::string, std::string> verbs;

		public:
			pingpong::modeset mset;
			std::string where;
			std::shared_ptr<pingpong::user> who;

			mode_line(client *parent_, const pingpong::modeset &mset_, const std::string &where_,
			const std::shared_ptr<pingpong::user> &who_, long stamp_):
				line(parent_, stamp_), mset(mset_), where(where_), who(who_) /*🦉*/ {}

			mode_line(client *parent_, const pingpong::mode_event &ev):
				mode_line(parent_, ev.mset, ev.where, ev.who, ev.stamp) {}

			virtual operator std::string() const override;
			virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
