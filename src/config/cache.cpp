#include "spjalla/config/cache.h"

namespace spjalla::config {
	std::string cache::get_string(const std::string &key) {
		if (registered.empty())
			register_defaults();
		return registered.at(key).default_value.string_();
	}

	ansi::color cache::get_color(const std::string &key) {
		if (registered.empty())
			register_defaults();
		return ansi::get_color(get_string(key));
	}

	bool cache::get_bool(const std::string &key) {
		if (registered.empty())
			register_defaults();
		return registered.at(key).default_value.bool_();
	}

	long cache::get_long(const std::string &key) {
		if (registered.empty())
			register_defaults();
		return registered.at(key).default_value.long_();
	}
}