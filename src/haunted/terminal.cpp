#include <stdexcept>
#include <string>

#include <unistd.h>

#include "haunted/terminal.h"

namespace haunted {
	terminal::terminal() {
		original = attrs = getattr();
	}

	terminal::~terminal() {
		reset();
	}

	termios terminal::getattr() {
		termios out;
		int result;
		if ((result = tcgetattr(STDIN_FILENO, &out)) < 0)
			throw std::runtime_error("tcgetattr returned " + std::to_string(result));

		return out;
	}

	void terminal::setattr(const termios &attrs) {
		int result;
		if ((result = tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs)) < 0)
			throw std::runtime_error("tcsetattr returned " + std::to_string(result));
	}

	void terminal::apply() {
		setattr(attrs);
	}

	void terminal::reset() {
		setattr(original);
		attrs = original;
	}

	void terminal::cbreak() {
		attrs.c_lflag &= ~(ECHO | ICANON);
		apply();
	}
}
