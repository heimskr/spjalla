#include <algorithm>

#include "pingpong/core/irc.h"

#include "spjalla/core/client.h"
#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"

#include "lib/formicine/ansi.h"

namespace spjalla::config {

	registered_map registered {};
	std::map<std::string, validator_fn> validators {};

	validation_result validate_long(const value &val) {
		return val.get_type() == value_type::long_? validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_string(const value &val) {
		return val.get_type() == value_type::string_? validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_bool(const value &val) {
		return val.get_type() == value_type::bool_? validation_result::valid : validation_result::bad_type;
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
	const validator_fn &validator, const applicator_fn &applicator, const std::string &description) {
		std::string combined = group + "." + key;

		if (registered.count(group + "." + key) > 0)
			return false;

		registered.insert({combined, default_key(combined, default_value, validator, applicator, description)});
		return true;
	}

	void apply_defaults(database &db) {
		for (auto &pair: registered)
			pair.second.apply(db, pair.second.default_value);
	}

	std::vector<std::string> starts_with(const std::string &str) {
		std::vector<std::string> out;
		for (const std::pair<std::string, default_key> &pair: registered) {
			const std::string &full = pair.first;
			const std::string &key = full.substr(full.find('.') + 1);
			if (key.find(str) == 0 || full.find(str) == 0)
				out.push_back(full);
		}

		return out;
	}

	void register_defaults() {
		// Appearance

		//// Bar
		register_key("appearance", "bar_background", "blood", validate_color, [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_bar_background(ansi::get_color(new_val.string_()));
		}, "The background color of the status bar and title bar.");

		register_key("appearance", "bar_foreground", "normal", validate_color, [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_bar_foreground(ansi::get_color(new_val.string_()));
		}, "The text color of the status bar and title bar.");

		//// Overlay
		register_key("appearance", "overlay_background", "verydark", validate_color,
		             [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_overlay_background(ansi::get_color(new_val.string_()));
		}, "The background color of the overlay window.");

		register_key("appearance", "overlay_foreground", "white", validate_color,
		             [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_overlay_foreground(ansi::get_color(new_val.string_()));
		}, "The text color of the overlay window.");

		//// Input
		register_key("appearance", "input_background", "normal", validate_color,
		             [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_input_background(ansi::get_color(new_val.string_()));
		}, "The background color of the input box.");

		register_key("appearance", "input_foreground", "normal", validate_color,
		             [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_input_foreground(ansi::get_color(new_val.string_()));
		}, "The text color of the input box.");

		//// Notices
		register_key("appearance", "notice_foreground", "magenta", validate_color, {},
			"The text color of names in notice messages.");

		// Completion

		register_key("completion", "ping_suffix", ":", validate_string, {},
			"The suffix to put after a user's name after tab completing their name in the first word of the message.");

		// Debug

		register_key("debug", "show_raw", false, validate_bool, [](database &db, const value &new_val) {
			db.get_parent().log_spam = new_val.bool_();
		}, "Whether to log raw input/output in the status window.");

		// Interface

		register_key("interface", "close_on_part", true, validate_bool, {},
			"Whether to close a channel's window after parting it.");

		// Messages

		register_key("messages", "direct_only", false, validate_bool, {},
			"Whether to count only messages that begin with your name as highlights.");

		register_key("messages", "highlight_notices", true, validate_bool, {},
			"Whether to treat all notices as highlights.");

		register_key("messages", "notices_in_status", true, validate_bool, {},
			"Whether non-channel notices should be displayed in the status window instead of a user window.");

		// Server

		register_key("server", "default_nick", pingpong::irc::default_nick, validate_string, {},
			"The nickname to use when connecting to a new server.");

		register_key("server", "default_user", pingpong::irc::default_user, validate_string,
		             [](database &db, const value &new_val) {
			db.get_parent().get_irc().username = new_val.string_();
		}, "The username to use when connecting to a new server.");

		register_key("server", "default_real", pingpong::irc::default_realname, validate_string,
		             [](database &db, const value &new_val) {
			db.get_parent().get_irc().realname = new_val.string_();
		}, "The real name to use when connecting to a new server.");
	}
}
