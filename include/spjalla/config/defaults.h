#ifndef SPJALLA_CONFIG_DEFAULTS_H_
#define SPJALLA_CONFIG_DEFAULTS_H_

#include <functional>
#include <unordered_map>

#include "spjalla/config/config.h"

namespace spjalla::config {
	using applicator = std::function<void(database &, const value &)>;
	using validator  = std::function<validation_result(const value &)>;

	struct default_key {
		std::string name, description;
		value default_value;
		validator validator;
		applicator on_set;

		default_key(const std::string &name_, const value &default_value_, const config::validator &validator_,
		const applicator &on_set_, const std::string &description_ = ""):
			name(name_), description(description_), default_value(default_value_), validator(validator_),
			on_set(on_set_) {}

		default_key(const std::string &name_, const value &default_value_, const config::validator &validator_,
		const std::string &description_ = ""):
			default_key(name_, default_value_, validator_, {}, description_) {}

		default_key(const std::string &name_, const value &default_value_, const config::applicator &on_set_,
		const std::string &description_ = ""):
			default_key(name_, default_value_, {}, on_set_, description_) {}

		default_key(const std::string &name_, const value &default_value_, const std::string &description_ = ""):
			default_key(name_, default_value_, {}, {}, description_) {}

		validation_result validate(const value &val) const {
			return validator? validator(val) : validation_result::valid;
		}

		void apply(database &db, const value &new_value) {
			if (on_set)
				on_set(db, new_value);
		}

		void apply(database &db) { apply(db, default_value); }
	};

	using registered_map = std::unordered_map<std::string, default_key>;

	/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
	 *  registers the key and returns true. */
	bool register_key(const std::string &group, const std::string &key, const value &default_val,
		const validator &validator_fn = {}, const applicator &on_set = {}, const std::string &description = "");

	/** Runs the applicators of all registered defaults with their default values. */
	void apply_defaults(database &db);

	/** Returns a vector of the names of all default keys whose full name or key name begins with a given string. */
	std::vector<std::string> starts_with(const std::string &);

	/** Registers the standard Spjalla configuration keys. */
	void register_defaults();

	extern registered_map registered;
}

#endif
