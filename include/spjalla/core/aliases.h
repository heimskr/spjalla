#ifndef SPJALLA_CORE_ALIASES_H_
#define SPJALLA_CORE_ALIASES_H_

#include <fstream>
#include <string>
#include <unordered_map>

#include "spjalla/core/input_line.h"

namespace spjalla {
	class aliases {
		private:
			std::string path;
			std::unordered_map<std::string, std::string> map;

		public:
			aliases(const std::string &path_ = ""): path(path_) {}
			
			/** Adds an alias. */
			void add_alias(const std::string &key, const std::string &expansion);

			/** Returns whether a given alias is known. */
			bool has_alias(const std::string &);
			
			/** Replaces any aliased command in an input_line with its expansion in-place. */
			input_line & expand(input_line &);
	};
}

#endif
