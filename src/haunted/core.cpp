#include <stdexcept>
#include <string>

#include <unistd.h>

#include "haunted/core.h"

namespace haunted {
	std::unique_ptr<termios> core::original, core::copy;

	termios & core::attrs() {
		if (!copy)
			getattr();
		return *copy;
	}

	termios core::getattr() {
		termios out;
		int result;
		if ((result = tcgetattr(STDIN_FILENO, &out)) < 0)
			throw std::runtime_error("tcgetattr returned " + std::to_string(result));
		
		if (!original) {
			// If we haven't called getattr before, store the result so we can restore it later.
			// Store a copy too, so we don't have to call tcgetattr every time a change is needed.
			original = std::make_unique<termios>(out);
			copy = std::make_unique<termios>(out);
		}
		
		return out;
	}

	void core::setattr(const termios &attrs) {
		int result;
		if ((result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs)) < 0)
			throw std::runtime_error("tcsetattr returned " + std::to_string(result));
	}

	void core::resetattr() {
		if (original) setattr(*original);
	}

	void core::cbreak() {
		termios &attr = attrs();
		attr.c_lflag &= ~ECHO;
		setattr(attr);
	}

	void core::cleanup() {
		resetattr();
	}
}
