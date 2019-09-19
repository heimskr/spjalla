#ifndef SPJALLA_LINES_MODE_H_
#define SPJALLA_LINES_MODE_H_

#include "pingpong/events/mode.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct mode_line: public haunted::ui::textline {
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
