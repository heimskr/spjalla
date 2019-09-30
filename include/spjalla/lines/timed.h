#ifndef SPJALLA_LINES_TIMED_H_
#define SPJALLA_LINES_TIMED_H_

#include "pingpong/core/pputil.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct timed_line: public haunted::ui::textline {
		std::string text;
		long stamp;

		timed_line(const std::string &text_, int continuation_ = 0, long stamp_ = pingpong::util::timestamp()):
			haunted::ui::textline(continuation_ + 11), text(text_), stamp(stamp_) {}

		virtual operator std::string() const override;
	};
}

#endif
