#ifndef SPJALLA_LINES_LINES_H_
#define SPJALLA_LINES_LINES_H_

#include <string>

#include "haunted/ui/textbox.h"
#include "formicine/ansi.h"

namespace spjalla::lines {
	/** Renders a UNIX timestamp as an hours-minutes-seconds set. */
	std::string render_time(long seconds);
}

#endif
