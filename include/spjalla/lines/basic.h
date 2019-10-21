#ifndef SPJALLA_LINES_BASIC_H_
#define SPJALLA_LINES_BASIC_H_

#include "pingpong/core/util.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct basic_line: public line {
		std::string text;

		basic_line(client *parent_, const std::string &text_, int continuation_ = 0,
		long stamp_ = pingpong::util::timestamp()):
			line(parent_, stamp_, continuation_), text(text_) {}

		basic_line(const std::string &text_, int continuation_ = 0, long stamp_ = pingpong::util::timestamp()):
			basic_line(nullptr, text_, continuation_, stamp_) {}

		virtual std::string render(ui::window *) override;
	};
}

#endif
