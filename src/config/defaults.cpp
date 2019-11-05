#include <algorithm>

#include <cstdint>

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

	validation_result validate_nonnegative(const value &val) {
		return val.get_type() == value_type::long_ && 0 <= val.long_()?
			validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_uint32(const value &val) {
		return val.get_type() == value_type::long_ && 0 <= val.long_() && val.long_() <= UINT32_MAX?
			validation_result::valid : validation_result::bad_type;
	}

	validation_result validate_int32nn(const value &val) {
		return val.get_type() == value_type::long_ && 0 <= val.long_() && val.long_() <= INT32_MAX?
			validation_result::valid : validation_result::bad_type;
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
		register_key("appearance", "bar_background", "blood", validate_color, APPLY_COLOR(bar_background),
			"The background color of the status bar and title bar.");

		register_key("appearance", "bar_foreground", "normal", validate_color, APPLY_COLOR(bar_foreground),
			"The text color of the status bar and title bar.");

		//// Overlay
		register_key("appearance", "overlay_background", "verydark", validate_color, APPLY_COLOR(overlay_background),
			"The background color of the overlay window.");

		register_key("appearance", "overlay_foreground", "white", validate_color, APPLY_COLOR(overlay_foreground),
			"The text color of the overlay window.");

		//// Input
		register_key("appearance", "input_background", "normal", validate_color, APPLY_COLOR(input_background),
			"The background color of the input box.");

		register_key("appearance", "input_foreground", "normal", validate_color, APPLY_COLOR(input_foreground),
			"The text color of the input box.");

		//// Notices
		register_key("appearance", "notice_foreground", "magenta", validate_color, CACHE_COLOR(notice_foreground),
			"The text color of names in notice messages.");

		// Behavior

		register_key("behavior", "answer_version_requests", true, validate_bool,
			CACHE_BOOL(behavior_answer_version_requests), "Whether to respond to CTCP VERSION requests.");

		register_key("behavior", "hide_version_requests", true, validate_bool,
			CACHE_BOOL(behavior_hide_version_requests), "Whether to handle CTCP VERSION requests silently.");

		// Completion

		register_key("completion", "ping_suffix", ":", validate_string, CACHE_STRING(completion_ping_suffix),
			"The suffix to put after a user's name after tab completing their name in the first word of the message.");

		// Debug

		register_key("debug", "show_raw", false, validate_bool, CACHE_BOOL(debug_show_raw),
			"Whether to log raw input/output in the status window.");

		// Interface

		register_key("interface", "close_on_part", true, validate_bool, {},
			"Whether to close a channel's window after parting it.");

		register_key("interface", "scroll_buffer", 0, validate_int32nn, [](database &db, const value &new_val) {
			db.get_parent().get_ui().set_scroll_buffer(static_cast<unsigned int>(new_val.long_()));
			db.get_parent().cache.interface_scroll_buffer = new_val.long_();
		}, "The number of lines to leave at the top when running /clear.");

		register_key("interface", "nick_format_bright", "^[cyan!]#n^[/f]", validate_string,
			CACHE_STRING(interface_nick_format_bright),
			"The format string for nicks in messages like joins. \"#n\" is replaced with the user's name.");

		register_key("interface", "nick_format", "^[cyan]#n^[/f]", validate_string, CACHE_STRING(interface_nick_format),
			"The format string for nicks in messages like quits and parts. \"#n\" is replaced with the user's name.");

		register_key("interface", "channel_format", "^b#c^B", validate_string, CACHE_STRING(interface_channel_format),
			"The format string for channels in messages like joins. \"#c\" is replaced with the channel's name.");

		// Messages

		register_key("messages", "direct_only", false, validate_bool, CACHE_BOOL(messages_direct_only),
			"Whether to count only messages that begin with your name as highlights.");

		register_key("messages", "highlight_notices", true, validate_bool, CACHE_BOOL(messages_highlight_notices),
			"Whether to treat all notices as highlights.");

		register_key("messages", "notices_in_status", true, validate_bool, CACHE_BOOL(messages_notices_in_status),
			"Whether non-channel notices should be displayed in the status window instead of a user window.");

		// Server

		register_key("server", "default_nick", pingpong::irc::default_nick, validate_string,
			CACHE_STRING(server_default_nick), "The nickname to use when connecting to a new server.");

		register_key("server", "default_user", pingpong::irc::default_user, validate_string,
		             [](database &db, const value &new_val) {
			db.get_parent().cache.server_default_user = db.get_parent().get_irc().username = new_val.string_();
		}, "The username to use when connecting to a new server.");

		register_key("server", "default_real", pingpong::irc::default_realname, validate_string,
		             [](database &db, const value &new_val) {
			db.get_parent().cache.server_default_real = db.get_parent().get_irc().realname = new_val.string_();
		}, "The real name to use when connecting to a new server.");
	}
}
