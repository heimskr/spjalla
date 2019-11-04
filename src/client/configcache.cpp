#include "spjalla/client/configcache.h"

namespace spjalla {
	std::string configcache::get_string(const std::string &key) {
		if (config::registered.empty())
			config::register_defaults();
		return config::registered.at(key).default_value.string_();
	}

	ansi::color configcache::get_color(const std::string &key) {
		if (config::registered.empty())
			config::register_defaults();
		return ansi::get_color(get_string(key));
	}

	bool configcache::get_bool(const std::string &key) {
		if (config::registered.empty())
			config::register_defaults();
		return config::registered.at(key).default_value.bool_();
	}

	long configcache::get_long(const std::string &key) {
		if (config::registered.empty())
			config::register_defaults();
		return config::registered.at(key).default_value.long_();
	}
}