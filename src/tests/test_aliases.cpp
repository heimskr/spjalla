#include "haunted/tests/test.h"

#include "spjalla/core/aliases.h"
#include "spjalla/core/util.h"

namespace spjalla::tests {
	void test_aliases(haunted::tests::testing &);
}

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_aliases(unit);	
}

namespace spjalla::tests {
	void test_aliases(haunted::tests::testing &unit) {
		using namespace std::string_literals;

		// aliases al("");

		// al.add_alias(

	}
}
