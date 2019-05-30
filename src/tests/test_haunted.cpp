#include <iostream>

#include "tests/haunted.h"
#include "haunted/terminal.h"

int main(int, char **) {
	using namespace haunted;
	terminal term;
	term.cbreak();

	std::string str;
	while (std::cin >> str) {
		std::cout << "[" << str << "] ";
	}

	std::cout << "Terminating." << std::endl;
}
