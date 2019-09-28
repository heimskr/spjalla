#include "tests/config.h"
#include "core/sputil.h"

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_config(unit);	
}

namespace spjalla::tests {
	void test_config(haunted::tests::testing &unit) {
		using namespace std::string_literals;

		config cfg;
		config::groupmap &reg = cfg.registered;

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

		unit.check("parse_string(\" \\\" bar\")", typeid(std::invalid_argument),
			"Invalid quote placement in string value", &config::parse_string, " \" bar"s);

		unit.check("parse_string(\"\\\"\")", typeid(std::invalid_argument),
			"Invalid length of string value", &config::parse_string, "\""s);
	}
}
