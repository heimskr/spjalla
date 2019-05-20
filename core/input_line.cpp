#include "core/input_line.h"

namespace spjalla {
	input_line::input_line(std::string full) {
		if (full[0] == '/') {
			size_t index, length = full.size();
			if (1 < length) {
				for (index = 1; index < length && full[index] != ' '; ++index);
				command = full.substr(1, index - 1);
				if (index != length)
					body = full.substr(index + 1);
			} else minimal = true;
		} else {
			body = full;
		}
	}

	input_line::operator std::string() const {
		if (is_command()) {
			return "Command[" + command + "], Body[" + body + "]";
		}
		
		return "Body[" + body + "]";
	}
}
