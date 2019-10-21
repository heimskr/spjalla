#ifndef SPJALLA_PLUGINS_LOGS_LOG_LINE_H_
#define SPJALLA_PLUGINS_LOGS_LOG_LINE_H_

#include "spjalla/lines/line.h"

namespace spjalla::plugins::logs {
	/**
	 * Represents messages like "Log opened on ...".
	 */
	struct log_line: public lines::line {
		std::string verb;

		log_line(client *parent_, const std::string &verb_, long stamp_):
			lines::line(parent_, stamp_, 0), verb(verb_) {}

		virtual std::string render(ui::window *) override;
	};
}

#endif