#ifndef SPJALLA_CONFIG_DEFAULTS_H_
#define SPJALLA_CONFIG_DEFAULTS_H_

#include <functional>
#include <unordered_map>

#include "config/config.h"

namespace spjalla::config {
	using applicator = std::function<void(database &, const value &)>;
	using validator  = std::function<validation_result(const value &)>;

	struct default_key {
		value default_value;
		validator validator;
		applicator on_set;

		default_key(const value &default_value_, const config::validator &validator_, const applicator &on_set_):
			default_value(default_value_), validator(validator_), on_set(on_set_) {}

		default_key(const value &default_value_, const config::validator &validator_):
			default_key(default_value_, validator_, {}) {}

		default_key(const value &default_value_, const config::applicator &on_set_):
			default_key(default_value_, {}, on_set_) {}

		default_key(const value &default_value_):
			default_key(default_value_, {}, {}) {}

		validation_result validate(const value &val) const {
			return validator? validator(val) : validation_result::valid;
		}

		void apply(database &db, const value &new_value) {
			if (on_set)
				on_set(db, new_value);
		}
	};

	using registered_map = std::unordered_map<std::string, default_key>;

	/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
	 *  registers the key and returns true. */
	bool register_key(const std::string &group, const std::string &key, const value &default_val,
		const validator &validator_fn = {}, const applicator &on_set = {});

	/** Runs the applicators of all registered defaults with their default values. */
	void apply_defaults(database &db);

	/** Registers the standard Spjalla configuration keys. */
	void register_defaults();


	extern registered_map registered;
}

#endif
