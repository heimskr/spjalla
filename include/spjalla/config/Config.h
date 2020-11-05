#ifndef SPJALLA_CONFIG_CONFIG_H_
#define SPJALLA_CONFIG_CONFIG_H_

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "spjalla/core/FlatDB.h"

#include "spjalla/config/Keys.h"
#include "spjalla/config/Validation.h"
#include "spjalla/config/Value.h"
#include "spjalla/tests/Config.h"

namespace Haunted::Tests { class Testing; }
namespace Spjalla::Tests { void testConfig(Haunted::Tests::Testing &); }

namespace Spjalla {
	class Client;
}

namespace Spjalla::Config {
	/**
	 * Represents an instance of a configuration database.
	 */
	class Database: public FlatDB {
		public:
			using   SubMap = std::map<std::string, Value>;
			using GroupMap = std::map<std::string, SubMap>;

		private:
			Client *parent;

			/** The in-memory copy of the config database. */
			GroupMap db {};

			/** Whether to allow unknown group+key combinations to be inserted into the database. */
			bool allowUnknown;

			static bool parseBool(const std::string &str);

			/** Throws a std::invalid_argument exception if a group+key pair is unknown and unknown group+key pairs
			 *  aren't allowed. */
			void ensureKnown(const std::string &group, const std::string &key) const noexcept(false);

		public:
			Database() = delete;
			Database(Client &parent_, bool allow_unknown): parent(&parent_), allowUnknown(allow_unknown) {}

			~Database() override = default;

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parseLongLine(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parseDoubleLine(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*(true|false|on|off|yes|no)$/. */
			static std::pair<std::string, bool> parseBoolLine(const std::string &);

			/** Attempts to split a "group.key" pair. Throws std::invalid_argument if there isn't exactly one period in
			 *  the string or if the area before or after the period contains nothing. */
			static std::pair<std::string, std::string> parsePair(const std::string &);

			/** Checks a value and returns its type. */
			static ValueType getValueType(std::string) noexcept;

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insert(const std::string &group, const std::string &key, const Value &, bool save = true);

			/** Inserts a value into the config database. Returns true if a preexisting value was overwritten. */
			bool insertAny(const std::string &group, const std::string &key, const std::string &, bool save = true);

			/** Removes a value from the config database and optionally applies the default value for the key if one has
			 *  been registered. Returns true if a value was present and removed, or false if no match was found. */
			bool remove(const std::string &group, const std::string &key, bool apply_default = true, bool save = true);

			/** Applies all settings, optionally with default settings where not overridden. */
			void applyAll(bool with_defaults);

			virtual std::pair<std::string, std::string> applyLine(const std::string &) override;

			virtual void applyAll() override { applyAll(true); }

			virtual void clearAll() override { db.clear(); }

			virtual bool empty() const override { return db.empty(); }

			/** Returns a value from the config database. If an unknown group+key pair is given and not present in the
			 *  database, a std::out_of_range exception is thrown. */
			Value & get(const std::string &group, const std::string &key);
			Value & getPair(const std::pair<std::string, std::string> &pair) { return get(pair.first, pair.second); }

			/** Returns whether a group name is present in the config database. */
			bool hasGroup(const std::string &) const;

			/** Returns whether a key name is present within a given group in the config database. */
			bool hasKey(const std::string &group, const std::string &key) const;

			/** Returns whether a group+key pair has been registered. */
			bool keyKnown(const std::string &group, const std::string &key) const;

			/** Returns the number of keys present under a group. If the group doesn't exist in the config database, the
			 *  function returns -1. */
			ssize_t keyCount(const std::string &group) const;

			/** Returns a copy of the config database with all default keys filled in if not already present. */
			GroupMap withDefaults() const;

			/** Stringifies the config database. */
			operator std::string() const override;

			GroupMap::iterator begin() { return db.begin(); }
			GroupMap::iterator end() { return db.end(); }

			Client * getParent() { return parent; }

			friend void Spjalla::Tests::test_config(Haunted::Tests::Testing &);
	};
}

#endif
