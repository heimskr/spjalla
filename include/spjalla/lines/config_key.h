#ifndef SPJALLA_LINES_CONFIG_KEY_H_
#define SPJALLA_LINES_CONFIG_KEY_H_

#include "pingpong/core/util.h"

#include "spjalla/config/config.h"
#include "spjalla/lines/line.h"

namespace spjalla::lines {
	struct config_key_line: public line {
		std::string key;
		config::value value;
		bool indent;

		config_key_line(client *parent_, const std::string &key_, const config::value &value_, bool indent_ = true,
		long stamp_ = pingpong::util::timestamp()):
			line(parent_, stamp_, (indent_? 4 : 0) + key_.length() + 3), key(key_), value(value_), indent(indent_) {}

		config_key_line(client *parent_, const std::pair<std::string, config::value> &pair, bool indent_ = true,
		long stamp_ = pingpong::util::timestamp()):
			config_key_line(parent_, pair.first, pair.second, indent_, stamp_) {}

		virtual operator std::string() const override;
	};
}

#endif
