#ifndef SPJALLA_LINES_ERROR_H_
#define SPJALLA_LINES_ERROR_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct error_line: public line {
		std::string message;
		long stamp;

		error_line(client *parent_, const std::string &message_, long stamp_ = pingpong::util::timestamp()):
			line(parent_, stamp_, ansi::length(lines::red_notice)), message(message_) {}

		virtual std::string render(ui::window *) override;
		virtual notification_type get_notification_type() const override { return notification_type::highlight; }
	};
}

#endif
