#include "tests/config.h"

int main(int, char **) {
	haunted::tests::testing unit;
	spjalla::tests::test_config(unit);	
}

namespace spjalla::tests {
	void test_config(haunted::tests::testing &unit) {
		config cfg;
		unit.check(cfg.registered.size(), 0UL, "registered.count()");
		cfg.register_key("group", "one", {"string"});
		unit.check(cfg.registered.size(), 1UL, "registered.count()");
		unit.check(cfg.registered.at("group").size(), 1UL, "group.count()");
		unit.check(cfg.registered.at("group").at("one").get_type(), config_type::string_, "group.one.type");
	}
}
