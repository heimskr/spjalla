#ifndef SPJALLA_LINES_ALIAS_H_
#define SPJALLA_LINES_ALIAS_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct alias_line: public line {
		std::string key, expansion;

		alias_line(client *parent_, const std::string &key_, const std::string &expansion_, long stamp_ = now()):
			line(parent_, stamp_), key(key_), expansion(expansion_) {}

		virtual operator std::string() const override;
	};
}

#endif
