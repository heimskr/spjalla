#ifndef SPJALLA_CORE_CONFIG_H_
#define SPJALLA_CORE_CONFIG_H_

#include <filesystem>
#include <string>
#include <map>
#include <utility>

#include "haunted/core/key.h"

#include "core/spopt.h"

namespace haunted::tests { class testing; }
namespace spjalla::tests { void test_config(haunted::tests::testing &); }

namespace spjalla {
	struct keys {
		static haunted::key toggle_overlay, switch_server, next_window, previous_window;
	};

	enum class config_type {invalid, long_, double_, string_};

	class config_value {
		// Boost? Never heard of it.
		private:
			config_type type;
			long long_value {0};
			double double_value {0.};
			std::string string_value {};

		public:
			config_value(long long_):     type(config_type::long_),   long_value(long_) {}
			config_value(double double_): type(config_type::double_), double_value(double_) {}
			config_value(const std::string &str_): type(config_type::string_), string_value(str_) {}
			config_value(int int_): config_value(static_cast<long>(int_)) {}
			config_value(const char *str_): config_value(std::string(str_)) {}

			config_type get_type() { return type; }
			long & long_();
			double & double_();
			std::string & string_();

			config_value & operator=(long);
			config_value & operator=(double);
			config_value & operator=(const std::string &);
			config_value & operator=(int n) { return *this = static_cast<long>(n); }
			config_value & operator=(const char *s) { return *this = std::string(s); }

			bool operator==(const config_value &) const;
			bool operator==(long) const;
			bool operator==(double) const;
			bool operator==(const std::string &) const;
			bool operator==(int n) const { return *this == static_cast<long>(n); }
			bool operator==(const char *s) const { return *this == std::string(s); }

			explicit operator std::string() const;
	};

	/**
	 * Represents an instance of a configuration database.
	 */
	class config {
		using   submap = std::map<std::string, config_value>;
		using groupmap = std::map<std::string, submap>;

		private:
			/** The in-memory copy of the config database. */
			groupmap db {};

			/** The path where the config database will be read from and written to. */
			std::filesystem::path filepath;

			/** Whether to allow unknown group+key combinations to be inserted into the database. */
			bool allow_unknown;

			/** Stores known option keys (the first element of the pair) under named groups (the key type of the map)
			 *  with a config_value indicating the type and default value. */
			static groupmap registered;

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

			/** Attempts to parse a string from a key-value pair. */
			static std::string parse_string(std::string);

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
			void read_db();

		public:
			config(bool allow_unknown_): allow_unknown(allow_unknown_) {}

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);

			/** Attempts to split a "group.key" pair. Throws std::invalid_argument if there isn't exactly one period in
			 *  the string or if the area before or after the period contains nothing. */
			static std::pair<std::string, std::string> parse_pair(const std::string &);

			/** Checks a value and returns its type. */
			static config_type get_value_type(std::string) noexcept;

			/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
			 *  registers the key and returns true. */
			static bool register_key(const std::string &group, const std::string &key, const config_value &default_val);

			/** Registers the standard Spjalla configuration keys. */
			static void register_defaults();

			/** Creates a config directory in the user's home directory if one doesn't already exist.
			 *  Returns true if the directory had to be created. */
			static bool ensure_config_dir(const std::string &name = DEFAULT_DATA_DIR);

			/** Ensures the config directory exists and creates a blank config database inside it if one doesn't already
			 *  exist. Returns true if the config database had to be created. */
			static bool ensure_config_db(const std::string &dbname,
			                             const std::string &dirname);

			/** Sets the cached config database path and replaces the cached database with the one stored at the path.
			 */
			void set_path(const std::string &dbname = DEFAULT_CONFIG_DB, const std::string &dirname = DEFAULT_DATA_DIR);

			/** Reads the config database from the filesystem if the in-memory copy is empty. */
			void read_if_empty(const std::string &dbname  = DEFAULT_CONFIG_DB,
			                   const std::string &dirname = DEFAULT_DATA_DIR);

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert(const std::string &group, const std::string &key, const config_value &);

			/** Returns a value from the config database. If an unknown group+key pair is given and not present in the
			 *  database, a std::out_of_range exception is thrown. */
			config_value & get(const std::string &group, const std::string &key);

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
