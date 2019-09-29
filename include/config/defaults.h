#ifndef SPJALLA_CONFIG_DEFAULTS_H_
#define SPJALLA_CONFIG_DEFAULTS_H_

#include <functional>
#include <unordered_map>

#include "config/config.h"

namespace spjalla::config {
	using applicator = std::function<void()>;

	struct default_key {
		database::validator validator;
		value default_value;
		applicator on_set;

		default_key(const database::validator &validator_, const value &default_value_, const applicator &on_set_):
			validator(validator_), default_value(default_value_), on_set(on_set_) {}

		default_key(const database::validator &validator_, const value &default_value_):
			default_key(validator_, default_value_, applicator([]() { })) {}

		validation_result validate(const value &val) {
			return validator? validator(val) : validation_result::valid;
		}
	};

	using registered_map = std::unordered_map<std::pair<std::string, std::string>, default_key>;
}

#endif
