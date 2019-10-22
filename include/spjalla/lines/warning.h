#ifndef SPJALLA_LINES_WARNING_H_
#define SPJALLA_LINES_WARNING_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class warning_line: public line {
		protected:
			virtual pingpong::server * get_associated_server() const override { return serv; }

		public:
			std::string message;
			pingpong::server *serv = nullptr;

			warning_line(client *parent_, const std::string &message_, long stamp_ = pingpong::util::timestamp()):
				line(parent_, stamp_, ansi::length(lines::yellow_notice)), message(message_) {}

			virtual std::string render(ui::window *) override;
			virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
