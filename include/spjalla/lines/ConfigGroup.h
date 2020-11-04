#ifndef SPJALLA_LINES_CONFIG_GROUP_H_
#define SPJALLA_LINES_CONFIG_GROUP_H_

#include "pingpong/core/Util.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct ConfigGroupLine: public Line {
		std::string group;

		ConfigGroupLine(Client *parent_, const std::string &group_, long stamp_ = PingPong::Util::timestamp()):
			Line(parent_, stamp_), group(group_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
