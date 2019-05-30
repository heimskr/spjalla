#include <chrono>
#include <thread>
#include <iostream>

#include "tests/haunted.h"
#include "haunted/core.h"

int main(int, char **) {
	using namespace haunted;
	core::cbreak();

	std::string str;
	while (std::cin >> str) {
		std::cout << "[" << str << "] ";
	}

	std::cout << "Cleaning up." << std::endl;
	core::cleanup();
	std::cout << "Terminating." << std::endl;
}
