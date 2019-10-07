#include <vector>

#include "spjalla/lines/line.h"
#include "spjalla/plugins/logs.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	std::unique_ptr<lines::line> logs_plugin::get_line(const std::string &str) {
		const size_t word_count = formicine::util::word_count(str);
	}
}
