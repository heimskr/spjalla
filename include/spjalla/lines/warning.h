#ifndef SPJALLA_LINES_WARNING_H_
#define SPJALLA_LINES_WARNING_H_

#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	struct warning_line: public haunted::ui::textline {
		std::string message;
		long stamp;

		warning_line(const std::string &message_, long stamp_ = pingpong::util::timestamp()):
			haunted::ui::textline(0), message(message_), stamp(stamp_) {}

		virtual operator std::string() const override;
	};
}

#endif
