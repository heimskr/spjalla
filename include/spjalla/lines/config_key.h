#ifndef SPJALLA_LINES_CONFIG_KEY_H_
#define SPJALLA_LINES_CONFIG_KEY_H_

#include "pingpong/core/util.h"

#include "spjalla/config/config.h"
#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct config_key_line: public haunted::ui::textline {
		std::string key;
		config::value value;
		bool indent;
		long stamp;

		config_key_line(const std::string &key_, const config::value &value_, bool indent_ = true,
		long stamp_ = pingpong::util::timestamp()):
			haunted::ui::textline((indent_? 4 : 0) + key_.length() + 3), key(key_), value(value_), indent(indent_),
			stamp(stamp_) {}

		config_key_line(const std::pair<std::string, config::value> &pair, bool indent_ = true,
		long stamp_ = pingpong::util::timestamp()):
			config_key_line(pair.first, pair.second, indent_, stamp_) {}

		virtual operator std::string() const override;
	};
}

#endif
