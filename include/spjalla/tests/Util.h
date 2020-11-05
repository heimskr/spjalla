#ifndef SPJALLA_TESTS_UTIL_H_
#define SPJALLA_TESTS_UTIL_H_

#include "haunted/tests/Test.h"

#include "spjalla/config/Config.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Tests {
	void test_util(Haunted::Tests::Testing &);

	size_t nth_index(const std::string &str, char find, int n) { return formicine::util::nth_index(str, find, n); }
}

#endif
