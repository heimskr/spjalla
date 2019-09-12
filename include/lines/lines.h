#ifndef SPJALLA_LINES_LINES_H_
#define SPJALLA_LINES_LINES_H_

#include <string>

namespace spjalla::lines {
	/** Renders a UNIX timestamp as an hours-minutes-seconds set. */
	std::string render_time(long seconds);
}

#endif
