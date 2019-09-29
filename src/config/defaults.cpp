#include "pingpong/core/irc.h"

#include "core/client.h"
#include "config/config.h"
#include "config/defaults.h"

#include "lib/formicine/ansi.h"

namespace spjalla::config {

	registered_map registered {};
	std::map<std::string, validator> validators {};

	validation_result validate_long(const value &val) {
		return val.get_type() == value_type::long_? validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_string(const value &val) {
		return val.get_type() == value_type::string_? validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_color(const value &val) {
		if (val.get_type() != value_type::string_)
			return validation_result::bad_type;

		const std::string &str = val.string_();
		for (const auto &color_pair: ansi::color_names) {
			if (color_pair.second == str)
				return validation_result::valid;
		}

		return validation_result::bad_value;
	}

	bool register_key(const std::string &group, const std::string &key, const value &default_value,
	const validator &validator_fn, const applicator &on_set) {
		std::string combined = group + "." + key;

		if (registered.count(group + "." + key) > 0)
			return false;

		registered.insert({combined, default_key(default_value, validator_fn, on_set)});
		return true;
	}

	void register_defaults() {
		register_key("server", "default_nick", pingpong::irc::default_nick);
		register_key("server", "default_user", pingpong::irc::default_user);
		register_key("server", "default_real", pingpong::irc::default_realname);

		register_key("appearance", "bar_color", "blood", validate_color, [](database &db, const value &new_value) {
			db.get_parent().get_ui().set_bar_background(ansi::get_color(new_value.string_()));
			DBG("New value: " << new_value.string_());
		});
	}
}
