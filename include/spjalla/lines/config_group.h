#ifndef SPJALLA_LINES_CONFIG_GROUP_H_
#define SPJALLA_LINES_CONFIG_GROUP_H_

#include "pingpong/core/pputil.h"

#include "lines/lines.h"

namespace spjalla::lines {
	struct config_group_line: public haunted::ui::textline {
		std::string group;
		long stamp;

		config_group_line(const std::string &group_, long stamp_ = pingpong::util::timestamp()):
			haunted::ui::textline(0), group(group_), stamp(stamp_) {}

		virtual operator std::string() const override;
	};
}

#endif
