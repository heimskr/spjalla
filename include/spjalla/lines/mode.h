#ifndef SPJALLA_LINES_MODE_H_
#define SPJALLA_LINES_MODE_H_

#include <unordered_map>

#include "pingpong/events/mode.h"

#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	class mode_line: public haunted::ui::textline {
		private:
			/** Maps a mode change (e.g., "+o", "-b") to a verb that indicates what the mode change means. */
			static std::unordered_map<std::string, std::string> verbs;

		public:
			pingpong::modeset mset;
			std::string where;
			std::shared_ptr<pingpong::user> who;
			long stamp;

			mode_line(const pingpong::modeset &mset_, const std::string &where_,
			const std::shared_ptr<pingpong::user> &who_,long stamp_):
				haunted::ui::textline(0), mset(mset_), where(where_), who(who_) /*🦉*/, stamp(stamp_) {}

			mode_line(const pingpong::mode_event &ev):
				mode_line(ev.mset, ev.where, ev.who, ev.stamp) {}

			virtual operator std::string() const override;
	};
}

#endif
