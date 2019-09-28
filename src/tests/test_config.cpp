#include "tests/config.h"
#include "core/sputil.h"

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_config(unit);	
}

namespace spjalla::tests {
	void test_config(haunted::tests::testing &unit) {
		using namespace std::string_literals;

		config cfg {true};
		config::groupmap &reg = cfg.registered;

		config::ensure_config_db();

		unit.check(reg.size(), 0UL, "registered.count()");
		cfg.register_key("group", "one", {"string"});
		unit.check(reg.size(), 1UL, "registered.count()");

		config::submap &group = reg.at("group");
		unit.check(group.size(), 1UL, "group.count()");

		config_value &one = group.at("one");
		unit.check(one.get_type(), config_type::string_, "group.one.type");

		unit.check({
			{{"foo=bar"s},    {"foo", "bar"}},
			{{"foo=bar"s},    {"foo", "bar"}},
			{{"foo = bar "s}, {"foo", "bar"}},
			{{"foo = bar"s},  {"foo", "bar"}},
			{{"foo =bar "s},  {"foo", "bar"}},
			{{" foo=bar "s},  {"foo", "bar"}},
		}, &config::parse_kv_pair, "config::parse_kv_pair");

		unit.check({
			{{"bar"s, false}, "bar"},
			{{"\\\"\\\\\\\t"s, false}, "\"\\\t"},
		}, &util::unescape, "util::unescape");

		unit.check({
			{{" \"bar\"  "s}, "bar"},
			{{" \"foo\\\"\"  "s}, "foo\""},
			{{" \"f\too\\\"\"  "s}, "f\too\""},
			{{""s}, ""},
			{{"\"\""s}, ""},
		}, &config::parse_string, "config::parse_string");

		unit.check("config::parse_string(\" \\\" bar\")", typeid(std::invalid_argument),
			"Invalid quote placement in string value", &config::parse_string, " \" bar"s);

		unit.check("config::parse_string(\"\\\"\")", typeid(std::invalid_argument),
			"Invalid length of string value", &config::parse_string, "\""s);

		unit.check({
			{{"42"}, config_type::long_},
			{{"42."}, config_type::double_},
			{{"\"foo\""}, config_type::string_},
			{{""}, config_type::string_},
			{{"\""}, config_type::invalid},
			{{"."}, config_type::double_},
			{{".."}, config_type::invalid},
		}, &config::get_value_type, "config::get_value_type");

		unit.check({
			{{"key=42."s}, {"key", 42.0}},
			{{"key=."s},   {"key", 0.0}},
			{{"key=.9"s},  {"key", 0.9}},
			{{"key=1."s},  {"key", 1.0}},
		}, &config::parse_double_line, "config::parse_double_line");

		unit.check(cfg.has_group("foo"), false, "has_group(\"foo\")");
		ansi::out << ansi::info << "Inserting " << haunted::tests::testing::stringify({"foo", "bar", "hello"}) << "\n";
		cfg.insert("foo", "bar", "hello");
		unit.check(cfg.has_group("foo"), true, "has_group(\"foo\")");
		unit.check(cfg.key_count("foo"), 1L, "key_count(\"foo\")");
		unit.check(cfg.key_known("foo", "has_default"), false, "key_known(\"foo\", \"has_default\")");
		ansi::out << ansi::info << "Registering " << ansi::bold("foo.has_default") << " with default "
		          << ansi::bold("42") << "\n";
		cfg.register_key("foo", "has_default", 42L);
		unit.check(cfg.key_known("foo", "has_default"), true, "key_known(\"foo\", \"has_default\")");
		config_value cv_42 {42L}, cv_hello {"hello"};
		unit.check({
			{{"foo"s, "has_default"s}, cv_42},
			{{"foo"s, "bar"s}, cv_hello},
		}, &config::get, &cfg, "cfg.get");

		unit.check({{{"foo.bar"s}, {"foo"s, "bar"s}}}, &config::parse_pair, "config::parse_pair");
		unit.check("config::parse_pair(\"\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, ""s);
		unit.check("config::parse_pair(\".\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, "."s);
		unit.check("config::parse_pair(\"foo.\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, "foo."s);
		unit.check("config::parse_pair(\".bar\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, ".bar"s);
		unit.check("config::parse_pair(\"foo..bar\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, "foo..bar"s);
		unit.check("config::parse_pair(\"foo.bar.baz\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, "foo.bar.baz"s);
		unit.check("config::parse_pair(\"..\")", typeid(std::invalid_argument), "Invalid group+key pair",
			&config::parse_pair, ".."s);
	}
}
