#include <fstream>
#include <string>
#include <vector>

#include "haunted/tests/test.h"
#include "spjalla/core/util.h"
#include "spjalla/util/backward_reader.h"

namespace spjalla::tests {
	void test_backwards_line(haunted::tests::testing &unit) {
		std::vector<std::string> original_lines {};
		std::string line;
		const int min = -10, max = 64;
		const bool write = true;

		if (write) {
			std::fstream stream {"/tmp/backwards_line", std::ios::out | std::ios::trunc | std::ios::in};
			for (int i = min; i < max; ++i) {
				line = std::to_string(1UL << i);
				original_lines.push_back(line);
				stream << line << "\n";
			}
			stream.close();
		} else {
			for (int i = min; i < max; ++i) {
				line = std::to_string(1UL << i);
				original_lines.push_back(line);
			}
		}

		std::vector<std::string> out;

		for (ssize_t chunk_size: {1, 2, 3, 4, 5, 6, 32, 33, 64, 128, 2048, 4096, 9999, 99999}) {
			std::cout << "\n["_d << ansi::bold(std::to_string(chunk_size)) << "]"_d << "\n";
			util::backward_reader reader {"/tmp/backwards_line", chunk_size};
			for (int i = min; i < max; ++i) {
				std::string line;
				bool result = reader.readline(line);
				unit.check(result, true, "result");
				unit.check(line, original_lines[max - 1 - i], "line (" + std::to_string(i) + " @ " +
					std::to_string(chunk_size) + ")");
			}
		}
	}
}

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_backwards_line(unit);	
}
