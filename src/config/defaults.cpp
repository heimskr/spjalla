#include <algorithm>

#include <cstdint>

#include "pingpong/core/IRC.h"

#include "spjalla/core/Client.h"
#include "spjalla/config/Config.h"
#include "spjalla/config/Defaults.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Config {
	RegisteredMap registered {};
	std::map<std::string, Validator_f> validators {};

	ValidationResult validateLong(const Value &val) {
		return val.getType() == ValueType::Long? ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateNonnegative(const Value &val) {
		return val.getType() == ValueType::Long && 0 <= val.long_()?
			ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateUint32(const Value &val) {
		return val.getType() == ValueType::Long && 0 <= val.long_() && val.long_() <= UINT32_MAX?
			ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateInt32nn(const Value &val) {
		return val.getType() == ValueType::Long && 0 <= val.long_() && val.long_() <= INT32_MAX?
			ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateString(const Value &val) {
		return val.getType() == ValueType::String? ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateBool(const Value &val) {
		return val.getType() == ValueType::Bool? ValidationResult::Valid : ValidationResult::BadType;
	}

	ValidationResult validateColor(const Value &val) {
		if (val.getType() != ValueType::String)
			return ValidationResult::BadType;

		const std::string &str = val.string_();
		for (const auto &color_pair: ansi::color_names) {
			if (color_pair.second == str)
				return ValidationResult::Valid;
		}

		return ValidationResult::BadValue;
	}

	bool RegisterKey(const std::string &group, const std::string &key, const Value &default_value,
	const Validator_f &validator, const Applicator_f &applicator, const std::string &description) {
		std::string combined = group + "." + key;

		if (registered.count(group + "." + key) > 0)
			return false;

		registered.insert({combined, DefaultKey(combined, default_value, validator, applicator, description)});
		return true;
	}

	bool unregister(const std::string &group, const std::string &key) {
		std::string combined = group + "." + key;
		if (registered.count(group + "." + key) == 0)
			return false;
		registered.erase(combined);
		return true;
	}

	void applyDefaults(Database &db) {
		for (auto &pair: registered)
			pair.second.apply(db, pair.second.defaultValue);
	}

	std::vector<std::string> startsWith(const std::string &str) {
		std::vector<std::string> out;
		for (const std::pair<std::string, DefaultKey> &pair: registered) {
			const std::string &full = pair.first;
			const std::string &key = full.substr(full.find('.') + 1);
			if (key.find(str) == 0 || full.find(str) == 0)
				out.push_back(full);
		}

		return out;
	}

	void registerDefaults() {
		// Appearance

		registerAppearance();

		// Behavior

		RegisterKey("behavior", "answer_version_requests", true, validateBool,
			CACHE_BOOL(behaviorAnswerVersionRequests), "Whether to respond to CTCP VERSION requests.");

		RegisterKey("behavior", "hide_version_requests", true, validateBool,
			CACHE_BOOL(behaviorHideVersionRequests), "Whether to handle CTCP VERSION requests silently.");

		// Completion

		RegisterKey("completion", "ping_suffix", ":", validateString, CACHE_STRING(completionPingSuffix),
			"The suffix to put after a user's name after tab completing their name in the first word of the message.");

		// Debug

		RegisterKey("debug", "show_raw", false, validateBool, CACHE_BOOL(debugShowRaw),
			"Whether to log raw input/output in the status window.");

		// Format

		registerFormat();

		// Interface

		RegisterKey("interface", "close_on_part", true, validateBool, CACHE_BOOL(interfaceCloseOnPart),
			"Whether to close a channel's window after parting it.");

		RegisterKey("interface", "scroll_buffer", 0, validateInt32nn, [](Database &db, const Value &new_val) {
			db.getParent()->getUI().setScrollBuffer(static_cast<unsigned int>(new_val.long_()));
			db.getParent()->cache.interfaceScrollBuffer = new_val.long_();
		}, "The number of lines to leave at the top when running /clear.");

		RegisterKey("interface", "show_motds", true, validateBool, CACHE_BOOL(interfaceShowMotds),
			"Whether to show server MOTDs.");

		// Messages

		RegisterKey("messages", "direct_only", false, validateBool, CACHE_BOOL(messagesDirectOnly),
			"Whether to count only messages that begin with your name as highlights.");

		RegisterKey("messages", "highlight_notices", true, validateBool, CACHE_BOOL(messagesHighlightNotices),
			"Whether to treat all notices as highlights.");

		RegisterKey("messages", "notices_in_status", true, validateBool, CACHE_BOOL(messagesNoticesInStatus),
			"Whether non-channel notices should be displayed in the status window instead of a user window.");

		// Server

		RegisterKey("server", "default_nick", PingPong::IRC::defaultNick, validateString,
			CACHE_STRING(serverDefaultNick), "The nickname to use when connecting to a new server.");

		RegisterKey("server", "default_real", PingPong::IRC::defaultRealname, validateString,
		             [](Database &db, const Value &new_val) {
			db.getParent()->cache.serverDefaultReal = db.getParent()->getIRC().realname = new_val.string_();
		}, "The real name to use when connecting to a new server.");

		RegisterKey("server", "default_user", PingPong::IRC::defaultUser, validateString,
		             [](Database &db, const Value &new_val) {
			db.getParent()->cache.serverDefaultUser = db.getParent()->getIRC().username = new_val.string_();
		}, "The username to use when connecting to a new server.");
	}

	void registerAppearance() {
		// Bar
		RegisterKey("appearance", "bar_background", "blood", validateColor, APPLY_COLOR(BarBackground),
			"The background color of the status bar and title bar.");

		RegisterKey("appearance", "bar_foreground", "normal", validateColor, APPLY_COLOR(BarForeground),
			"The text color of the status bar and title bar.");

		// Overlay
		RegisterKey("appearance", "overlay_background", "verydark", validateColor, APPLY_COLOR(OverlayBackground),
			"The background color of the overlay window.");

		RegisterKey("appearance", "overlay_foreground", "white", validateColor, APPLY_COLOR(OverlayForeground),
			"The text color of the overlay window.");

		// Input
		RegisterKey("appearance", "input_background", "normal", validateColor, APPLY_COLOR(InputBackground),
			"The background color of the input box.");

		RegisterKey("appearance", "input_foreground", "normal", validateColor, APPLY_COLOR(InputForeground),
			"The text color of the input box.");

		// Notices
		RegisterKey("appearance", "notice_foreground", "magenta", validateColor, CACHE_COLOR(NoticeForeground),
			"The text color of names in notice messages.");

		// Miscellaneous
		RegisterKey("appearance", "allow_empty_hats", false, validateBool, CACHE_BOOL(appearanceAllowEmptyHats),
			"Whether to use a blank hat string instead of a space when a user has no hats.");
	}

	void registerFormat() {
		RegisterKey("format", "action", "$header$ $message$", validateString, CACHE_STRING(formatAction),
			"The format string for actions. Available variables: header, message.");

		RegisterKey("format", "bang", "^d-^b!^D^d-^D", validateString, CACHE_STRING(formatBang),
			"The format string for neutral bangs. Bangs are the little pieces of text in a line that indicate the "
			"urgency of the line.");

		RegisterKey("format", "bang_bad", "^[red]^d-^D!^D^d-^D^[/f]", validateString, CACHE_STRING(formatBangBad),
			"The format string for bangs that indicate an error.");

		RegisterKey("format", "bang_good", "^[green]^d-^D!^D^d-^D^[/f]", validateString, CACHE_STRING(formatBangGood),
			"The format string for bangs that indicate success.");

		RegisterKey("format", "bang_warn", "^[yellow]^d-^D!^D^d-^D^[/f]", validateString, CACHE_STRING(formatBangWarn),
			"The format string for bangs that indicate a warning.");

		RegisterKey("format", "channel", "^b$raw_channel$^B", validateString, CACHE_STRING(formatChannel),
			"The format string for channels in messages like joins. Available variables: raw_channel.");

		RegisterKey("format", "header_action", "^b* $hats$$nick$^0", validateString,
			CACHE_STRING(formatHeaderAction),
			"The format string for headers in actions. Available variables: hats, nick.");

		RegisterKey("format", "header_privmsg", "^d<^D$hats$$nick$^d>^D", validateString,
			CACHE_STRING(formatHeaderPrivmsg),
			"The format string for headers in privmsgs. Available variables: hats, nick.");

		RegisterKey("format", "header_notice", "^[magenta]-$hats$$nick$^0^[magenta]-^0", validateString,
			CACHE_STRING(formatHeaderNotice),
			"The format string for headers in notices. Available variables: hats, nick.");

		RegisterKey("format", "join", "$!$ $who$^0 joined $channel$^0",
			validateString, CACHE_STRING(formatJoin),
			"The format string for joins. Available variables: -!-, -!!-, -!?-, channel, who.");

		RegisterKey("format", "kick", "$!$ $whom$^0 was kicked from $channel$^0 by $who$^0 ^d[^D$reason$^0^d]^0",
			validateString, CACHE_STRING(formatKick),
			"The format string for kicks. Available variables: -!-, -!!-, -!?-, channel, reason, who, whom.");

		RegisterKey("format", "kick_self", "$bad!$ You were kicked from $channel$^0 by $who$^0 ^d[^D$reason$^0^d]"
			"^0", validateString, CACHE_STRING(formatKickSelf),
			"The format string for kicks when you're the kicked user. Available variables: -!-, -!!-, -!?-, channel, "
			"reason, who, whom.");

		RegisterKey("format", "message_action", "$raw_message$", validateString,
			CACHE_STRING(formatMessageAction),
			"The format string for messages in actions. Available variables: hats, nick.");

		RegisterKey("format", "message_privmsg", "$raw_message$", validateString,
			CACHE_STRING(formatMessagePrivmsg),
			"The format string for messages in privmsgs. Available variables: hats, nick.");

		RegisterKey("format", "message_notice", "$raw_message$", validateString,
			CACHE_STRING(formatMessageNotice),
			"The format string for messages in notices. Available variables: hats, nick.");

		RegisterKey("format", "notice", "$header$ $message$", validateString, CACHE_STRING(formatNotice),
			"The format string for notices. Available variables: header, notice.");

		RegisterKey("format", "nick_action", "$raw_nick$", validateString, CACHE_STRING(formatNickAction),
			"The format string for nicks in actions. Available variables: raw_nick.");

		RegisterKey("format", "nick_change", "$old$ is now known as $new$", validateString,
			CACHE_STRING(formatNickChange), "The format string for nick changes. Available variables: new, old.");

		RegisterKey("format", "nick_general_bright", "^[cyan!]$raw_nick$^[/f]", validateString,
			CACHE_STRING(formatNickGeneralBright),
			"The format string for nicks in messages like joins. Available variables: raw_nick.");

		RegisterKey("format", "nick_general", "^[cyan]$raw_nick$^[/f]", validateString,
			CACHE_STRING(formatNickGeneral),
			"The format string for nicks in messages like quits and parts. Available variables: raw_nick.");

		RegisterKey("format", "nick_privmsg", "$raw_nick$", validateString, CACHE_STRING(formatNickPrivmsg),
			"The format string for nicks in privmsgs. Available variables: raw_nick.");

		RegisterKey("format", "nick_notice", "$raw_nick$", validateString, CACHE_STRING(formatNickNotice),
			"The format string for nicks in notices. Available variables: raw_nick.");

		RegisterKey("format", "part", "$!$ $who$^0 left $channel$^0 ^d[^D$reason$^0^d]^0", validateString,
			CACHE_STRING(formatPart),
			"The format string for parts. Available variables: -!-, -!!-, -!?-, channel, reason, who.");

		RegisterKey("format", "privmsg", "$header$ $message$", validateString, CACHE_STRING(formatPrivmsg),
			"The format string for privmsgs. Available variables: header, message.");

		RegisterKey("format", "reason", "$raw_reason$", validateString, CACHE_STRING(formatReason),
			"The format string for reasons. Available variables: raw_reason.");

		RegisterKey("format", "quit", "$!$ $who$^0 quit ^d[^D$reason$^0^d]^0",
			validateString, CACHE_STRING(formatQuit),
			"The format string for quits. Available variables: -!-, -!!-, -!?-, reason, who.");

		RegisterKey("format", "timestamp", "%H^d:^D%M^d:^D%S", validateString, CACHE_STRING(formatTimestamp),
			"The format string for timestamps. Uses strftime syntax. Shouldn't exceed 32 characters when rendered.");

		RegisterKey("format", "topic", "$raw_topic$", validateString, CACHE_STRING(formatTopic),
			"The format string for topics. Available variables: raw_topic.");

		RegisterKey("format", "topic_is", "$!$ Topic for $channel$ is $topic$", validateString,
			CACHE_STRING(formatTopicIs), "The format string for topic notices. Available variables: -!-, -!!-, -!?-, "
			"channel, topic.");

		RegisterKey("format", "topic_change", "$!$ $who$ changed the topic of $channel$ to $topic$", validateString,
			CACHE_STRING(formatTopicChange), "The format string for topic changes. Available variables: -!-, -!!-, "
			"-!?-, channel, topic, who.");
	}
}
