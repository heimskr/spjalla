#ifndef SPJALLA_LINES_RAW_H_
#define SPJALLA_LINES_RAW_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct raw_line: public line {
		std::string text;
		bool is_bad = false;
		/** True if the line is for content that was sent to the server, false if the content was from the server. */
		bool is_out = false;

		raw_line(client *parent_, const std::string &text_, bool is_out_ = false, bool is_bad_ = false,
		long stamp_ = now()):
			line(parent_, stamp_, 3), text(text_), is_bad(is_bad_), is_out(is_out_) {}

		operator std::string() const override;
	};
}

#endif
