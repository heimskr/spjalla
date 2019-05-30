#include <stdexcept>
#include <string>

#include <unistd.h>

#include "haunted/haunted.h"

namespace haunted {
	std::unique_ptr<termios> haunted::original;

	termios haunted::getattr() {
		termios out;
		int result;
		if ((result = tcgetattr(STDIN_FILENO, &out)) < 0)
			throw std::runtime_error("tcgetattr returned " + std::to_string(result));
		
		if (!original) {
			// If we haven't called getattr before, store the result so we can restore it later.
			original = std::make_unique<termios>(out);
		}
		
		return out;
	}

	void haunted::setattr(const termios &attrs) {
		int result;
		if ((result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs)) < 0)
			throw std::runtime_error("tcsetattr returned " + std::to_string(result));
	}

	void haunted::resetattr() {
		if (original) setattr(*original);
	}

	void haunted::cbreak() {
		termios attr = getattr();
		attr.c_lflag &= ~ECHO;
	}

	void haunted::cleanup() {
		resetattr();
	}
}
