#ifndef SPJALLA_CONFIG_DEFAULTS_H_
#define SPJALLA_CONFIG_DEFAULTS_H_

#include <functional>
#include <unordered_map>

#include "spjalla/config/config.h"

namespace spjalla::config {
	using applicator_fn = std::function<void(database &, const value &)>;
	using validator_fn  = std::function<validation_result(const value &)>;

	struct default_key {
		std::string name, description;
		value default_value;
		validator_fn validator;
		applicator_fn applicator;

		default_key(const std::string &name_, const value &default_value_, const validator_fn &validator_,
		const applicator_fn &applicator_, const std::string &description_ = ""):
			name(name_), description(description_), default_value(default_value_), validator(validator_),
			applicator(applicator_) {}

		default_key(const std::string &name_, const value &default_value_, const validator_fn &validator_,
		const std::string &description_ = ""):
			default_key(name_, default_value_, validator_, {}, description_) {}

		default_key(const std::string &name_, const value &default_value_, const applicator_fn &applicator_,
		const std::string &description_ = ""):
			default_key(name_, default_value_, {}, applicator_, description_) {}

		default_key(const std::string &name_, const value &default_value_, const std::string &description_ = ""):
			default_key(name_, default_value_, {}, {}, description_) {}

		validation_result validate(const value &val) const {
			return validator? validator(val) : validation_result::valid;
		}

		void apply(database &db, const value &new_value) {
			if (applicator)
				applicator(db, new_value);
		}

		void apply(database &db) { apply(db, default_value); }
	};

	using registered_map = std::unordered_map<std::string, default_key>;

	/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
	 *  registers the key and returns true. */
	bool register_key(const std::string &group, const std::string &key, const value &default_val,
		const validator_fn & = {}, const applicator_fn & = {}, const std::string &description = "");

	/** Runs the applicators of all registered defaults with their default values. */
	void apply_defaults(database &db);

	/** Returns a vector of the names of all default keys whose full name or key name begins with a given string. */
	std::vector<std::string> starts_with(const std::string &);

	/** Registers the standard Spjalla configuration keys. */
	void register_defaults();

	extern registered_map registered;

	validation_result validate_long(const value &);
	validation_result validate_nonnegative(const value &);
	validation_result validate_uint32(const value &);
	validation_result validate_int32nn(const value &);
	validation_result validate_string(const value &);
	validation_result validate_bool(const value &);
	validation_result validate_color(const value &);
}

#endif
