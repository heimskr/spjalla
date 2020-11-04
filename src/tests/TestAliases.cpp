#include "haunted/tests/Test.h"

#include "spjalla/core/Aliases.h"
#include "spjalla/core/Util.h"

namespace Spjalla::Tests {
	void test_aliases(Haunted::Tests::Testing &);
}

int main(int, char **) {
	Haunted::Tests::Testing unit;
	Spjalla::Tests::test_aliases(unit);	
}

namespace Spjalla::Tests {
	void test_aliases(Haunted::Tests::Testing &unit) {
		using Haunted::Tests::Testing;
		using namespace std::string_literals;

		Aliases al;

		ansi::out << ansi::info << "Adding alias " << Testing::stringify(std::pair("foo", "/bar one")) << ansi::endl;
		al.addAlias("foo", "/bar one");

		ansi::out << ansi::info << "Line: " << "\""_bd << "/foo two three"_b << "\""_bd << ansi::endl;
		InputLine line("/foo two three");

		unit.check(line.isCommand(), true, "line.isCommand()");
		unit.check(line.command, "foo"s, "line.command");
		unit.check(line.body, "two three"s, "line.body");
		ansi::out << ansi::info << "Expanding with " << Testing::stringify(std::pair("foo", "/bar one")) << ansi::endl;
		al.expand(line);
		unit.check(line.isCommand(), true, "line.isCommand()");
		unit.check(line.command, "bar"s, "line.command");
		unit.check(line.body, "one two three"s, "line.body");
		unit.check(line.args[0], "one"s, "line.args[0]");
		unit.check(line.args[1], "two"s, "line.args[1]");
		unit.check(line.args[2], "three"s, "line.args[2]");
		unit.check(line.args.size(), 3UL, "line.args.size()");
		ansi::out << ansi::info << "Adding alias " << Testing::stringify(std::pair("bar", "hey there")) << ansi::endl;
		al.addAlias("bar", "hey there");
		ansi::out << ansi::info << "Expanding with " << Testing::stringify(std::pair("bar", "hey there")) << ansi::endl;
		al.expand(line);
		unit.check(line.isCommand(), false, "line.isCommand()");
		unit.check(line.command, ""s, "line.command");
		unit.check(line.body, "hey there one two three"s, "line.body");
		ansi::out << ansi::info << "Setting line to " << Testing::stringify("/baz multiple   spaces") << ansi::endl;
		line = {"/baz multiple   spaces"};
		ansi::out << ansi::info << "Adding alias " << Testing::stringify(std::pair("baz", "more  than one,  "))
		          << ansi::endl;
		al.addAlias("baz", "more  than one,  ");
		unit.check(line.isCommand(), true, "line.isCommand()");
		unit.check(line.command, "baz"s, "line.command");
		unit.check(line.body, "multiple   spaces"s, "line.body");
		unit.check(line.args[0], "multiple"s, "line.args[0]");
		unit.check(line.args[1], "spaces"s, "line.args[1]");
		unit.check(line.args.size(), 2UL, "line.args.size()");
		ansi::out << ansi::info << "Expanding with " << Testing::stringify(std::pair("baz", "more  than one,  "))
		          << ansi::endl;
		al.expand(line);
		unit.check(line.isCommand(), false, "line.isCommand()");
		unit.check(line.command, ""s, "line.command");
		unit.check(line.body, "more  than one,   multiple   spaces"s, "line.body");
		ansi::out << ansi::info << "Setting line to " << Testing::stringify("/quux hey there") << ansi::endl;
		line = {"/quux hey there"};
		unit.check(line.isCommand(), true, "line.isCommand()");
		unit.check(line.command, "quux"s, "line.command");
		unit.check(line.body, "hey there"s, "line.body");
		unit.check(line.args[0], "hey"s, "line.args[0]");
		unit.check(line.args[1], "there"s, "line.args[1]");
		unit.check(line.args.size(), 2UL, "line.args.size()");
		ansi::out << ansi::info << "Expanding non-existent alias" << ansi::endl;
		al.expand(line);
		unit.check(line.isCommand(), true, "line.isCommand()");
		unit.check(line.command, "quux"s, "line.command");
		unit.check(line.body, "hey there"s, "line.body");
		unit.check(line.args[0], "hey"s, "line.args[0]");
		unit.check(line.args[1], "there"s, "line.args[1]");
		unit.check(line.args.size(), 2UL, "line.args.size()");
	}
}
