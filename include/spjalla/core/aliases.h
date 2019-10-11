#ifndef SPJALLA_CORE_ALIASES_H_
#define SPJALLA_CORE_ALIASES_H_

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "spjalla/core/flatdb.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/options.h"

namespace spjalla {
	class aliases: public flatdb {
		private:
			std::filesystem::path filepath;
			std::unordered_map<std::string, std::string> db;

		public:
			aliases() {}

			~aliases() override = default;

			/** Adds an alias. */
			void add_alias(const std::string &key, const std::string &expansion);

			/** Returns whether a given alias is known. */
			bool has_alias(const std::string &);
			
			/** Replaces any aliased command in an input_line with its expansion in-place. */
			input_line & expand(input_line &);

			/** Returns a pair representing the key and the value. */
			virtual std::pair<std::string, std::string> apply_line(const std::string &) override;

			virtual void clear_all() override { db.clear(); }
			virtual bool empty() const override { return db.empty(); }

			/** Inserts an alias into the database. Returns true if a preexisting value was overwritten. */
			bool insert(const std::string &key, const std::string &expansion, bool save = true);

			/** Removes a value from the alias database. Returns true if a value was present and removed, or false
			 *  if no match was found. */
			bool remove(const std::string &key, bool save = true);

			/** Returns the alias corresponding to a key. If there's nothing present for the key, the function throws
			 *  std::out_of_range. */
			std::string get(const std::string &);

			auto  begin() { return db.begin(); }
			auto    end() { return db.end();   }
			size_t size() { return db.size();  }

			operator std::string() const override;
	};
}

#endif
