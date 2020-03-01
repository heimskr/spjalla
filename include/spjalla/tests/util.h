#ifndef SPJALLA_TESTS_UTIL_H_
#define SPJALLA_TESTS_UTIL_H_

#include "haunted/tests/test.h"

#include "spjalla/config/config.h"

#include "lib/formicine/futil.h"

namespace spjalla::tests {
	void test_util(Haunted::tests::testing &);

	size_t nth_index(const std::string &str, char find, int n) { return formicine::util::nth_index(str, find, n); }
}

#endif
