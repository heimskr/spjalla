#ifndef SPJALLA_CONFIG_CONFIG_H_
#define SPJALLA_CONFIG_CONFIG_H_

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "core/spopt.h"

#include "config/keys.h"
#include "config/validation.h"
#include "config/value.h"

namespace haunted::tests { class testing; }
namespace spjalla::tests { void test_config(haunted::tests::testing &); }

namespace spjalla::config {
	/**
	 * Represents an instance of a configuration database.
	 */
	class database {
		public:
			using   submap  = std::map<std::string, value>;
			using groupmap  = std::map<std::string, submap>;

		private:
			/** The in-memory copy of the config database. */
			groupmap db {};

			/** The path where the config database will be read from and written to. */
			std::filesystem::path filepath;

			/** Whether to allow unknown group+key combinations to be inserted into the database. */
			bool allow_unknown;

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

			/** Given a data directory name and a config database name, this returns the full path of the config
			 *  database. */
			static std::filesystem::path get_db_path(const std::string &dbname  = DEFAULT_CONFIG_DB,
			                                         const std::string &dirname = DEFAULT_DATA_DIR);

			/** Throws a std::invalid_argument exception if a group+key pair is unknown and unknown group+key pairs
			 *  aren't allowed. */
			void ensure_known(const std::string &group, const std::string &key) const noexcept(false);

			/** Writes the database to the cached file path. */
			void write_db();

			/** Read the database from the cached file path. */
			void read_db(bool clear = true);

		public:
			database(bool allow_unknown_): allow_unknown(allow_unknown_) {}

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);

			/** Attempts to split a "group.key" pair. Throws std::invalid_argument if there isn't exactly one period in
			 *  the string or if the area before or after the period contains nothing. */
			static std::pair<std::string, std::string> parse_pair(const std::string &);

			/** Attempts to parse a string from a key-value pair. */
			static std::string parse_string(std::string);

			/** Checks a value and returns its type. */
			static value_type get_value_type(std::string) noexcept;

			/** Creates a config directory in the user's home directory if one doesn't already exist.
			 *  Returns true if the directory had to be created. */
			static bool ensure_config_dir(const std::string &name = DEFAULT_DATA_DIR);

			/** Ensures the config directory exists and creates a blank config database inside it if one doesn't already
			 *  exist. Returns true if the config database had to be created. */
			static bool ensure_config_db(const std::string &dbname  = DEFAULT_CONFIG_DB,
			                             const std::string &dirname = DEFAULT_DATA_DIR);

			/** Sets the cached config database path and replaces the cached database with the one stored at the path.
			 */
			void set_path(const std::string &dbname = DEFAULT_CONFIG_DB, const std::string &dirname = DEFAULT_DATA_DIR);

			/** Reads the config database from the filesystem if the in-memory copy is empty. */
			void read_if_empty(const std::string &dbname  = DEFAULT_CONFIG_DB,
			                   const std::string &dirname = DEFAULT_DATA_DIR);

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert(const std::string &group, const std::string &key, const value &, bool save = true);

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert_any(const std::string &group, const std::string &key, const std::string &, bool save = true);

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
			operator std::string() const;

			groupmap::iterator begin() { return db.begin(); }
			groupmap::iterator end() { return db.end(); }

			friend void spjalla::tests::test_config(haunted::tests::testing &);
	};
}

#endif
