#ifndef SPJALLA_CONFIG_DEFAULTS_H_
#define SPJALLA_CONFIG_DEFAULTS_H_

#include <functional>
#include <unordered_map>

#include "spjalla/config/Config.h"

namespace Spjalla::Config {
	using Applicator_f = std::function<void(Database &, const Value &)>;
	using Validator_f  = std::function<ValidationResult(const Value &)>;

	struct DefaultKey {
		std::string name, description;
		Value defaultValue;
		Validator_f validator;
		Applicator_f applicator;

		DefaultKey(const std::string &name_, const Value &default_value, const Validator_f &validator_,
		const Applicator_f &applicator_, const std::string &description_ = ""):
			name(name_), description(description_), defaultValue(default_value), validator(validator_),
			applicator(applicator_) {}

		DefaultKey(const std::string &name_, const Value &default_value, const Validator_f &validator_,
		const std::string &description_ = ""):
			DefaultKey(name_, default_value, validator_, {}, description_) {}

		DefaultKey(const std::string &name_, const Value &default_value, const Applicator_f &applicator_,
		const std::string &description_ = ""):
			DefaultKey(name_, default_value, {}, applicator_, description_) {}

		DefaultKey(const std::string &name_, const Value &default_value, const std::string &description_ = ""):
			DefaultKey(name_, default_value, {}, {}, description_) {}

		ValidationResult validate(const Value &val) const {
			return validator? validator(val) : ValidationResult::Valid;
		}

		void apply(Database &db, const Value &new_value) {
			if (applicator)
				applicator(db, new_value);
		}

		void apply(Database &db) { apply(db, defaultValue); }
	};

	using RegisteredMap = std::unordered_map<std::string, DefaultKey>;

#define APPLY_COLOR(name) [](Database &db, const Value &new_val) { \
	db.getParent()->getUI().set##name(db.getParent()->cache.appearance##name = ansi::get_color(new_val.string_())); }

#define CACHE_COLOR(name) [](Database &db, const Value &new_val) { \
	db.getParent()->cache.appearance##name = ansi::get_color(new_val.string_()); }

#define CACHE_BOOL(name)   [](Database &db, const Value &new_val) { db.getParent()->cache.name = new_val.bool_();   }
#define CACHE_LONG(name)   [](Database &db, const Value &new_val) { db.getParent()->cache.name = new_val.long_();   }
#define CACHE_STRING(name) [](Database &db, const Value &new_val) { db.getParent()->cache.name = new_val.string_(); }

	/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
	 *  registers the key and returns true. */
	bool RegisterKey(const std::string &group, const std::string &key, const Value &default_val,
		const Validator_f & = {}, const Applicator_f & = {}, const std::string &description = "");

	/** Attempts to unregister a key. Returns true if the key existed and was removed. */
	bool unregister(const std::string &group, const std::string &key);

	/** Runs the applicators of all registered defaults with their default values. */
	void apply_defaults(Database &db);

	/** Returns a vector of the names of all default keys whose full name or key name begins with a given string. */
	std::vector<std::string> startsWith(const std::string &);

	/** Registers the standard Spjalla configuration keys. */
	void registerDefaults();
	void registerAppearance();
	void registerFormat();

	extern RegisteredMap registered;

	ValidationResult validateLong(const Value &);
	ValidationResult validateNonnegative(const Value &);
	ValidationResult validateUint32(const Value &);
	ValidationResult validateInt32nn(const Value &);
	ValidationResult validateString(const Value &);
	ValidationResult validateBool(const Value &);
	ValidationResult validateColor(const Value &);
}

#endif
