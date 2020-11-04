#include "spjalla/config/Cache.h"

namespace Spjalla::Config {
	std::string Cache::getString(const std::string &key) {
		if (registered.empty())
			registerDefaults();
		return registered.at(key).defaultValue.string_();
	}

	ansi::color Cache::getColor(const std::string &key) {
		if (registered.empty())
			registerDefaults();
		return ansi::get_color(getString(key));
	}

	bool Cache::getBool(const std::string &key) {
		if (registered.empty())
			registerDefaults();
		return registered.at(key).defaultValue.bool_();
	}

	long Cache::getLong(const std::string &key) {
		if (registered.empty())
			registerDefaults();
		return registered.at(key).defaultValue.long_();
	}
}