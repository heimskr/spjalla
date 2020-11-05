#ifndef SPJALLA_LINES_ALIAS_H_
#define SPJALLA_LINES_ALIAS_H_

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct AliasLine: public Line {
		std::string key, expansion;

		AliasLine(Client *parent_, const std::string &key_, const std::string &expansion_, long stamp_ = now()):
			Line(parent_, stamp_), key(key_), expansion(expansion_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
