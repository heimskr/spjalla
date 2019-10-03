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
		using haunted::tests::testing;

		using namespace std::string_literals;

		aliases al("");

		ansi::out << ansi::info << "Adding alias " << testing::stringify(std::pair("foo", "/bar one")) << ansi::endl;
		al.add_alias("foo", "/bar one");

		ansi::out << ansi::info << "Line: " << "\""_bd << "/foo two three"_b << "\""_bd << ansi::endl;
		input_line line("/foo two three");

		unit.check(line.is_command(), true, "line.is_command()");
		unit.check(line.command, "foo"s, "line.command");
		unit.check(line.body, "two three"s, "line.body");
		ansi::out << ansi::info << "Expanding with " << testing::stringify(std::pair("foo", "/bar one")) << ansi::endl;
		al.expand(line);
		unit.check(line.is_command(), true, "line.is_command()");
		unit.check(line.command, "bar"s, "line.command");
		unit.check(line.body, "one two three"s, "line.body");
		unit.check(line.args[0], "one"s, "line.args[0]");
		unit.check(line.args[2], "three"s, "line.args[2]");

		ansi::out << ansi::info << "Adding alias " << testing::stringify(std::pair("bar", "hey there")) << ansi::endl;
		al.add_alias("bar", "hey there");
		ansi::out << ansi::info << "Expanding with " << testing::stringify(std::pair("bar", "hey there")) << ansi::endl;
		al.expand(line);
		unit.check(line.is_command(), false, "line.is_command()");
		unit.check(line.command, ""s, "line.command");
		unit.check(line.body, "hey there one two three"s, "line.body");


	}
}
