#include "tests/util.h"
#include "spjalla/core/util.h"

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_util(unit);	
}

namespace spjalla::tests {
	void test_util(haunted::tests::testing &unit) {
		using namespace std::string_literals;

		unit.check({
			{{"foo"s, 0},           { 0,  0}},
			{{"foo"s, 1},           { 0,  1}},
			{{"foo"s, 2},           { 0,  2}},
			{{"foo"s, 3},           { 0,  3}},
			{{"foo"s, 4},           {-2, -1}},
			{{"foo bar"s, 3},       { 0,  3}},
			{{"foo bar"s, 4},       { 1,  0}},
			{{"foo bar"s, 6},       { 1,  2}},
			{{"foo bar"s, 7},       { 1,  3}},
			{{"foo bar"s, 8},       {-3, -1}},
			{{"foo  bar"s, 4},      {-2, -1}},
			{{"foo "s, 4},          {-2, -1}},
			{{"foo    "s, 4},       {-2, -1}},
			{{"foo    "s, 7},       {-2, -1}},
			{{" foo"s, 0},          {-1, -1}},
			{{"  foo"s, 0},         {-1, -1}},
			{{"  foo"s, 1},         {-1, -1}},
			{{"  foo"s, 2},         { 0,  0}},
			{{"foo  bar  baz"s, 4}, {-2, -1}},
			{{"foo  bar  baz"s, 9}, {-3, -1}},
			{{""s, 0},              {-1, -1}},
			{{""s, 1},              {-1, -1}},
			{{""s, 2},              {-1, -1}},
			{{" "s, 0},             {-1, -1}},
			{{" "s, 1},             {-1, -1}},
			{{" "s, 2},             {-1, -1}},
		}, &util::word_indices, "util::word_indices");

		unit.check({
			{{"foo bar baz"s, 0},  0},
			{{"foo bar baz"s, 1},  4},
			{{"foo bar baz"s, 2},  8},
			{{"foo bar baz"s, 3}, 11},
			{{"foo bar baz"s, 4}, 11},
			{{"foo"s,         0},  0},
			{{"foo"s,         1},  3},
			{{" "s,           0},  1},
			{{" "s,           1},  1},
			{{"  "s,          0},  2},
			{{"  "s,          1},  2},
		}, &util::index_of_word, "util::index_of_word");

		unit.check({
			{{"foo bar baz"s, 0},  3},
			{{"foo bar baz"s, 1},  7},
			{{"foo bar baz"s, 2}, 11},
			{{"foo bar baz"s, 3}, 11},
			{{"foo bar baz"s, 4}, 11},
			{{"foo"s,         0},  3},
			{{"foo"s,         1},  3},
			{{" "s,           0},  1},
			{{" "s,           1},  1},
			{{"  "s,          0},  2},
			{{"  "s,          1},  2},
		}, &util::last_index_of_word, "util::last_index_of_word");
	}
}
