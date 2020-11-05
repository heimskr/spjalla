#ifndef SPJALLA_PLUGINS_LOGS_LOGLINE_H_
#define SPJALLA_PLUGINS_LOGS_LOGLINE_H_

#include "spjalla/lines/Line.h"

namespace Spjalla::Plugins::Logs {
	/**
	 * Represents messages like "Log opened on ...".
	 */
	struct LogLine: public Lines::Line {
		std::string verb;

		LogLine(Client *parent_, const std::string &verb_, long stamp_):
			Lines::Line(parent_, stamp_, 0), verb(verb_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif