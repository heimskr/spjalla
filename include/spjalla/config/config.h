#ifndef SPJALLA_CONFIG_CONFIG_H_
#define SPJALLA_CONFIG_CONFIG_H_

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "spjalla/core/flatdb.h"

#include "spjalla/config/keys.h"
#include "spjalla/config/validation.h"
#include "spjalla/config/value.h"

namespace haunted::tests { class testing; }
namespace spjalla::tests { void test_config(haunted::tests::testing &); }

namespace spjalla {
	class client;
}


namespace spjalla::config {

	/**
	 * Represents an instance of a configuration database.
	 */
	class database: public flatdb {
		public:
			using   submap = std::map<std::string, value>;
			using groupmap = std::map<std::string, submap>;

		private:
			client &parent;

			/** The in-memory copy of the config database. */
			groupmap db {};

			/** Whether to allow unknown group+key combinations to be inserted into the database. */
			bool allow_unknown;

			static bool parse_bool(const std::string &str);

			/** Throws a std::invalid_argument exception if a group+key pair is unknown and unknown group+key pairs
			 *  aren't allowed. */
			void ensure_known(const std::string &group, const std::string &key) const noexcept(false);

		public:
			database(client &parent_, bool allow_unknown_): parent(parent_), allow_unknown(allow_unknown_) {}

			~database() override = default;

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*(true|false|on|off|yes|no)$/. */
			static std::pair<std::string, bool> parse_bool_line(const std::string &);

			/** Attempts to split a "group.key" pair. Throws std::invalid_argument if there isn't exactly one period in
			 *  the string or if the area before or after the period contains nothing. */
			static std::pair<std::string, std::string> parse_pair(const std::string &);

			/** Checks a value and returns its type. */
			static value_type get_value_type(std::string) noexcept;

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert(const std::string &group, const std::string &key, const value &, bool save = true);

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert_any(const std::string &group, const std::string &key, const std::string &, bool save = true);

			/** Removes a value from the config database and optionally applies the default value for the key if one has
			 *  been registered. Returns true if a value was present and removed, or false if no match was found. */
			bool remove(const std::string &group, const std::string &key, bool apply_default = true, bool save = true);

			/** Applies all settings, optionally with default settings where not overridden. */
			void apply_all(bool with_defaults);

			virtual void apply_line(const std::string &) override;

			virtual void apply_all() override { apply_all(true); }

			virtual void clear_all() override { db.clear(); }

			virtual bool empty() const override { return db.empty(); }

			/** Returns a value from the config database. If an unknown group+key pair is given and not present in the
			 *  database, a std::out_of_range exception is thrown. */
			value & get(const std::string &group, const std::string &key);
			value & get_pair(const std::pair<std::string, std::string> &pair) { return get(pair.first, pair.second); }

			/** Returns whether a group name is present in the config database. */
			bool has_group(const std::string &) const;

			/** Returns whether a key name is present within a given group in the config database. */
			bool has_key(const std::string &group, const std::string &key) const;

			/** Returns whether a group+key pair has been registered. */
			bool key_known(const std::string &group, const std::string &key) const;

			/** Returns the number of keys present under a group. If the group doesn't exist in the config database, the
			 *  function returns -1. */
			ssize_t key_count(const std::string &group) const;

			/** Returns a copy of the config database with all default keys filled in if not already present. */
			groupmap with_defaults() const;

			/** Stringifies the config database. */
			operator std::string() const override;

			groupmap::iterator begin() { return db.begin(); }
			groupmap::iterator end() { return db.end(); }

			client & get_parent() { return parent; }

			friend void spjalla::tests::test_config(haunted::tests::testing &);
	};
}

#endif
