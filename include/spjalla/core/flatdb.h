#ifndef SPJALLA_CORE_FLATDB_H_
#define SPJALLA_CORE_FLATDB_H_

#include <filesystem>
#include <string>
#include <utility>

#include "spjalla/core/options.h"

namespace spjalla {
	class flatdb {
		protected:
			/** The path where the database will be read from and written to. */
			std::filesystem::path filepath;

			/** Given a data directory name and a database name, this returns the full path of the database. */
			static std::filesystem::path get_db_path(const std::string &dbname,
			                                         const std::string &dirname = DEFAULT_DATA_DIR);

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

			/** Attempts to parse a string from a key-value pair. */
			static std::string parse_string(std::string);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);

		public:
			virtual ~flatdb() {};

			/** Applies a line of read input. */
			virtual void apply_line(const std::string &) = 0;

			/** Applies all database items. Useful for the configuration database, which has applicators, but less
			 *  useful for the alias database which just reads items and doesn't immediately act on them. */
			virtual void apply_all() {}

			/** Clears all database items. */
			virtual void clear_all() = 0;

			/** Returns whether the database is empty. */
			virtual bool empty() const = 0;

			/** Sets the cached database path and replaces the cached database with the one stored at the path. */
			void set_path(const std::string &dbname, bool apply = true, const std::string &dirname = DEFAULT_DATA_DIR);

			/** Reads the database from the filesystem if the in-memory copy is empty. */
			void read_if_empty(const std::string &dbname, bool apply = true,
			                   const std::string &dirname = DEFAULT_DATA_DIR);

			/** Writes the database to the cached file path. */
			void write_db();

			/** Reads the database from the cached file path. */
			void read_db(bool apply = true, bool clear = true);

			/** Stringifies the database. */
			virtual operator std::string() const = 0;

			/** Creates a config directory in the user's home directory if one doesn't
			 *  already exist. Returns true if the directory had to be created. */
			static bool ensure_dir(const std::string &name = DEFAULT_DATA_DIR);

			/** Ensures the config directory exists and creates a blank config database inside it if
			 *  one doesn't already exist. Returns true if the config database had to be created. */
			static bool ensure_db(const std::string &dbname, const std::string &dirname = DEFAULT_DATA_DIR);
	};
}

#endif
