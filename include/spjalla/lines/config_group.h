#ifndef SPJALLA_LINES_CONFIG_GROUP_H_
#define SPJALLA_LINES_CONFIG_GROUP_H_

#include "pingpong/core/util.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct config_group_line: public line {
		std::string group;

		config_group_line(client *parent_, const std::string &group_, long stamp_ = pingpong::util::timestamp()):
			line(parent_, stamp_), group(group_) {}

		virtual operator std::string() const override;
	};
}

#endif
