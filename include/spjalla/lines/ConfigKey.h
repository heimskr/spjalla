#ifndef SPJALLA_LINES_CONFIG_KEY_H_
#define SPJALLA_LINES_CONFIG_KEY_H_

#include "pingpong/core/Util.h"

#include "spjalla/config/Config.h"
#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct ConfigKeyLine: public Line {
		std::string key;
		Config::Value value;
		bool indent;

		ConfigKeyLine(Client *parent_, const std::string &key_, const Config::Value &value_, bool indent_ = true,
		long stamp_ = PingPong::Util::timestamp()):
			Line(parent_, stamp_, (indent_? 4 : 0) + key_.length() + 3), key(key_), value(value_), indent(indent_) {}

		ConfigKeyLine(Client *parent_, const std::pair<std::string, Config::Value> &pair, bool indent_ = true,
		long stamp_ = PingPong::Util::timestamp()):
			ConfigKeyLine(parent_, pair.first, pair.second, indent_, stamp_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
