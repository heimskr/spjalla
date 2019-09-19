#include <iostream>
#include <iomanip>

#include "tests/haunted.h"
#include "haunted/core/terminal.h"

int main(int, char **) {
	using namespace haunted;
	terminal term;
	term.cbreak();

	char ch;
	while (term >> ch) {
		std::cout << "\r" << std::setw(3) << std::left << std::setfill(' ') << static_cast<int>(ch) << " " << ch << std::endl;
	}

	std::cout << "Terminating." << std::endl;
}
