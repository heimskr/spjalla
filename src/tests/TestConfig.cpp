#include "spjalla/tests/Config.h"
#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"

int main(int, char **) {
	Haunted::Tests::Testing unit;
	Spjalla::Tests::test_config(unit);	
}

namespace Spjalla::Tests {
	void test_config(Haunted::Tests::Testing &unit) {
		using namespace std::string_literals;

		Client client;

		Config::Database cfg {client, true};
		Config::Database::GroupMap &reg = cfg.db;

		Config::Database::ensureDB("testdb");

/*
		unit.check(reg.size(), 0UL, "registered.count()");
		cfg.RegisterKey("group", "one", {"string"});
		unit.check(reg.size(), 1UL, "registered.count()");
//*/

		Config::Database::SubMap &group = reg.at("group");
		unit.check(group.size(), 1UL, "group.count()");

		Config::Value &one = group.at("one");
		unit.check(one.getType(), Config::ValueType::String, "group.one.type");

		unit.check({
			{{"foo=bar"s},    {"foo", "bar"}},
			{{"foo=bar"s},    {"foo", "bar"}},
			{{"foo = bar "s}, {"foo", "bar"}},
			{{"foo = bar"s},  {"foo", "bar"}},
			{{"foo =bar "s},  {"foo", "bar"}},
			{{" foo=bar "s},  {"foo", "bar"}},
		}, &Config::Database::parseKVPair, "Config::Database::parseKVPair");

		unit.check({
			{{"bar"s, false}, "bar"},
			{{"\\\"\\\\\\\t"s, false}, "\"\\\t"},
		}, &Util::unescape, "util::unescape");

		unit.check({
			{{" \"bar\"  "s}, "bar"},
			{{" \"foo\\\"\"  "s}, "foo\""},
			{{" \"f\too\\\"\"  "s}, "f\too\""},
			{{""s}, ""},
			{{"\"\""s}, ""},
		}, &Config::Database::parseString, "Config::Database::parseString");

		unit.check("Config::Database::parseString(\" \\\" bar\")", typeid(std::invalid_argument),
			"Invalid quote placement in string value", &Config::Database::parseString, " \" bar"s);

		unit.check("Config::Database::parseString(\"\\\"\")", typeid(std::invalid_argument),
			"Invalid length of string value", &Config::Database::parseString, "\""s);

		unit.check({
			{{"42"}, Config::ValueType::Long},
			{{"42."}, Config::ValueType::Double},
			{{"\"foo\""}, Config::ValueType::String},
			{{""}, Config::ValueType::String},
			{{"\""}, Config::ValueType::Invalid},
			{{"."}, Config::ValueType::Double},
			{{".."}, Config::ValueType::Invalid},
		}, &Config::Database::getValueType, "Config::Database::getValueType");

		unit.check({
			{{"key=42."s}, {"key", 42.0}},
			{{"key=."s},   {"key", 0.0}},
			{{"key=.9"s},  {"key", 0.9}},
			{{"key=1."s},  {"key", 1.0}},
		}, &Config::Database::parseDoubleLine, "Config::Database::parseDoubleLine");

		unit.check(cfg.hasGroup("foo"), false, "hasGroup(\"foo\")");
		ansi::out << ansi::info << "Inserting " << Haunted::Tests::Testing::stringify({"foo", "bar", "hello"}) << "\n";
		cfg.insert("foo", "bar", "hello");
		unit.check(cfg.hasGroup("foo"), true, "hasGroup(\"foo\")");
		unit.check(cfg.keyCount("foo"), 1L, "keyCount(\"foo\")");
		unit.check(cfg.keyKnown("foo", "has_default"), false, "keyKnown(\"foo\", \"has_default\")");
		ansi::out << ansi::info << "Registering " << ansi::bold("foo.has_default") << " with default "
		          << ansi::bold("42") << "\n";
		// cfg.RegisterKey("foo", "has_default", 42L);
		// unit.check(cfg.keyKnown("foo", "has_default"), true, "keyKnown(\"foo\", \"has_default\")");
		Config::Value cv_42 {42L}, cv_hello {"hello"};

		unit.check({
			{{"foo"s, "has_default"s}, cv_42},
			{{"foo"s, "bar"s}, cv_hello},
		}, &Config::Database::get, &cfg, "cfg.get");

		unit.check({{{"foo.bar"s}, {"foo"s, "bar"s}}}, &Config::Database::parsePair, "Config::Database::parsePair");
		unit.check("Config::Database::parsePair(\"\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, ""s);
		unit.check("Config::Database::parsePair(\".\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, "."s);
		unit.check("Config::Database::parsePair(\"foo.\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, "foo."s);
		unit.check("Config::Database::parsePair(\".bar\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, ".bar"s);
		unit.check("Config::Database::parsePair(\"foo..bar\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, "foo..bar"s);
		unit.check("Config::Database::parsePair(\"foo.bar.baz\")", typeid(std::invalid_argument),
			"Invalid group+key pair", &Config::Database::parsePair, "foo.bar.baz"s);
		unit.check("Config::Database::parsePair(\"..\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&Config::Database::parsePair, ".."s);
	}
}
