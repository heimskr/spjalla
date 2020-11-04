#ifndef SPJALLA_LINES_BASIC_H_
#define SPJALLA_LINES_BASIC_H_

#include "pingpong/core/Util.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct BasicLine: public Line {
		std::string text;

		BasicLine(Client *parent_, const std::string &text_, int base_continuation = 0,
		long stamp_ = PingPong::Util::timestamp()):
			Line(parent_, stamp_, base_continuation), text(text_) {}

		BasicLine(const std::string &text_, int base_continuation = 0, long stamp_ = PingPong::Util::timestamp()):
			BasicLine(nullptr, text_, base_continuation, stamp_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
