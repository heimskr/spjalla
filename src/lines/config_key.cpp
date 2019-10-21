#include "spjalla/lines/config_key.h"

namespace spjalla::lines {
	std::string config_key_line::render(ui::window *) {
		return (indent? "    " : "") + key + " = "_d + ansi::bold(std::string(value));
	}
}
