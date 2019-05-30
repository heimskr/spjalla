#include <stdexcept>
#include <string>

#include <unistd.h>

#include "haunted/terminal.h"

namespace haunted {
	terminal::~terminal() {
		if (current) delete current;
		if (original) {
			setattr(*original);
			delete original;
		}
	}

	termios & terminal::attrs() {
		if (!current)
			getattr();
		return *current;
	}

	termios terminal::getattr() {
		termios out;
		int result;
		if ((result = tcgetattr(STDIN_FILENO, &out)) < 0)
			throw std::runtime_error("tcgetattr returned " + std::to_string(result));
		
		if (!original) {
			// If we haven't called getattr before, store the result so we can restore it later.
			// Store a working copy too so we don't have to call tcgetattr every time a change is needed.
			original = new termios(out);
			current  = new termios(out);
		}
		
		return out;
	}

	void terminal::setattr(const termios &attrs) {
		int result;
		if ((result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs)) < 0)
			throw std::runtime_error("tcsetattr returned " + std::to_string(result));
	}

	void terminal::resetattr() {
		if (original) setattr(*original);
	}

	void terminal::cbreak() {
		termios &attr = attrs();
		attr.c_lflag &= ~(ECHO | ICANON);
		setattr(attr);
	}
}
