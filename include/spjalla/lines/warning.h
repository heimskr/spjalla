#ifndef SPJALLA_LINES_WARNING_H_
#define SPJALLA_LINES_WARNING_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct warning_line: public line {
		std::string message;
		long stamp;

		warning_line(client *parent_, const std::string &message_, long stamp_ = pingpong::util::timestamp()):
			line(parent_, 0), message(message_), stamp(stamp_) {}

		virtual operator std::string() const override;
		virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
