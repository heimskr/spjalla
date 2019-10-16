#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "haunted/core/key.h"

#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/mode.h"
#include "pingpong/events/nick.h"
#include "pingpong/events/notice.h"
#include "pingpong/events/part.h"
#include "pingpong/events/privmsg.h"
#include "pingpong/events/quit.h"
#include "pingpong/events/topic.h"
#include "pingpong/events/topic_updated.h"

#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"

#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/plugin_host.h"
#include "spjalla/core/util.h"

#include "spjalla/lines/notice.h"
#include "spjalla/lines/privmsg.h"

#include "spjalla/plugins/plugin.h"
#include "spjalla/plugins/logs.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins::logs {
	logs_plugin::~logs_plugin() {
		while (!filemap.empty())
			close(filemap.begin()->first);
	}

	void logs_plugin::log(const log_pair &pair, const std::string &message, const std::string &type) {
		(get_stream(pair) << pingpong::util::timestamp() << precision_suffix() << " " << type << " " << message << "\n").flush();
	}

	void logs_plugin::log(std::shared_ptr<pingpong::user> user, const std::string &message, const std::string &type) {
		for (ui::window *window: parent->get_ui().windows_for_user(user)) {
			switch (window->type) {
				case ui::window_type::channel:
					log({user->serv, window->chan->name}, message, type);
					break;
				case ui::window_type::user:
					log({user->serv, window->user->name}, message, type);
					close({user->serv, window->user->name});
					break;
				default: ;
			}
		}
	}

	void logs_plugin::log(pingpong::local_event *event) {
		if (event->serv->get_parent() == &parent->get_irc())
			log({event->serv, event->where}, *event);
	}

	std::fstream & logs_plugin::get_stream(const log_pair &pair) {
		if (filemap.count(pair) == 1)
			return filemap.at(pair);

		const std::filesystem::path path = get_path(pair);
		const bool existed = std::filesystem::exists(path);
		std::fstream new_stream(path, std::ios::app | std::ios::in | std::ios::out);
		if (!new_stream)
			throw std::runtime_error("Couldn't open file stream for " + std::string(path));
		if (!existed)
			new_stream << pingpong::util::timestamp() << precision_suffix() << " created" << "\n";
		(new_stream << pingpong::util::timestamp() << precision_suffix() << " opened" << "\n").flush();
		std::vector<std::string> lines;
		filemap.insert({pair, std::move(new_stream)});
		return filemap.at(pair);
	}

	bool logs_plugin::close(const log_pair &pair) {
		if (filemap.count(pair) == 0)
			return false;

		std::fstream &stream = get_stream(pair);
		(stream << pingpong::util::timestamp() << precision_suffix() << " closed\n").flush();
		stream.close();
		filemap.erase(pair);
		return true;
	}

	std::filesystem::path logs_plugin::get_path(const log_pair &pair) {
		const std::filesystem::path dir = base / sanitize_filename(pair.first->id);
		if (!std::filesystem::exists(dir))
			std::filesystem::create_directories(dir);

		return dir / sanitize_filename(pair.second);
	}

	std::string & logs_plugin::sanitize_filename(std::string &str) {
		const static std::string invalid = "\x07/ \\,.:*\0";
		for (auto iter = str.begin(); iter != str.end(); ++iter) {
			if (invalid.find(*iter) != std::string::npos)
				*iter = '_';
		}
		return str;
	}

	std::string logs_plugin::sanitize_filename(const std::string &str) {
		std::string out {str};
		sanitize_filename(out);
		return out;
	}

	constexpr char logs_plugin::precision_suffix() {
		switch (pingpong::util::precision) {
			case 1'000'000'000L: return 'n';
			case 1'000'000L: return 'u';
			case 1'000L: return 'm';
			default:  return 's';
		}
	}

	std::chrono::microseconds logs_plugin::parse_stamp(std::string stamp_str) {
		if (stamp_str.empty())
			throw std::invalid_argument("Timestamp is empty");

		char suffix = 's';
		const static std::string suffixes = "nums";
		if (suffixes.find(stamp_str.back()) != std::string::npos) {
			suffix = stamp_str.back();
			stamp_str.pop_back();
		}

		long parsed;
		if (!formicine::util::parse_long(stamp_str, parsed))
			throw std::invalid_argument("Invalid timestamp: " + stamp_str);

		switch (suffix) {
			case 's':
				return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(parsed));
			case 'm':
				return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::milliseconds(parsed));
			case 'u':
				return std::chrono::microseconds(parsed);
			case 'n':
				return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::nanoseconds(parsed));
			default:
				throw std::invalid_argument("Timestamp has an invalid precision identifier");
		}
	}
		
	void logs_plugin::preinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		config::register_key("logs", "autoclean", true, config::validate_bool, {},
			"Whether /restore should exclude lines that indicate when the log was opened, created or closed.");
		config::register_key("logs", "default_restore", 128, config::validate_long, {},
			"The default number of scrollback lines to restore with /restore.");
		config::register_key("logs", "enabled", true, config::validate_bool, {}, "Whether to enable logs.");

		base = util::get_home() / DEFAULT_DATA_DIR / "logs";
		if (!std::filesystem::exists(base)) {
			DBG("Created log directory.");
			std::filesystem::create_directories(base);
		} else if (!std::filesystem::is_directory(base)) {
			throw std::runtime_error("A file already exists at " + std::string(base));
		}
	}

	void logs_plugin::postinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }

		client->add({"clean", {0, 1, true, [this](pingpong::server *, const input_line &) { clean(); }, {}}});

		client->add({"restore", {0, 1, true, [this](pingpong::server *serv, const input_line &il) {
			restore(serv, il);
		}, {}}});

		pingpong::events::listen<pingpong::join_event>([&](pingpong::join_event *event) {
			log({event->serv, event->chan->name}, event->who->name, "join");
		});


		pingpong::events::listen<pingpong::kick_event>([&](pingpong::kick_event *event) {
			log({event->serv, event->chan->name}, event->who->name + " " + event->whom->name + " :" + event->content,
				"kick");
		});


		pingpong::events::listen<pingpong::mode_event>([&](pingpong::mode_event *event) {
			// Ignore self mode changes.
			if (event->get_name() == event->where)
				return;

			const std::string extra = !event->mset.extra.empty()? " " + event->mset.extra : "";
			log({event->serv, event->where}, event->get_name() + " " + event->serv->get_nick() + " " +
				event->mset.mode_str() + extra, "mode");
		});


		pingpong::events::listen<pingpong::nick_event>([&](pingpong::nick_event *event) {
			log(event->who, event->content + " " + event->who->name, "nick");
		});


		pingpong::events::listen<pingpong::notice_event>([=](pingpong::notice_event *event) {
			if (event->serv->get_parent() != &client->get_irc())
				return;

			lines::notice_line line = event->is_channel() || event->speaker? lines::notice_line(client, *event) :
				lines::notice_line(client, event->serv->id, "*", event->serv->get_nick(), event->content, event->stamp,
					{}, true);

			// A '$' logfile is for messages received from the server itself.
			const std::string where = event->is_channel()? event->where : (event->speaker? event->speaker->name : "$");
			std::string nick = event->serv->get_nick();
			if (nick.empty())
				nick = "!"; // A '!', in this case, represents you before your nick has been set.

			log({event->serv, where}, formicine::util::ltrim(line.hat_str() + line.name) + " " + nick + " " +
				(line.is_action()? "*" : ":") + line.trimmed(line.message), "notice");
		});


		pingpong::events::listen<pingpong::part_event>([&](pingpong::part_event *event) {
			log({event->serv, event->chan->name}, event->who->name + " :" + event->content, "part");

			// If you parted the channel, close the channel's stream.
			if (event->who->is_self())
				close({event->serv, event->chan->name});
		});


		pingpong::events::listen<pingpong::privmsg_event>([=](pingpong::privmsg_event *event) {
			if (event->serv->get_parent() != &client->get_irc()) {
				DBG("Different parent IRCs: " << event->serv->get_parent() << " vs. " << &client->get_irc());
				return;
			}

			lines::privmsg_line line {client, *event};
			log({event->serv, event->is_channel()? event->where : event->speaker->name},
				formicine::util::ltrim(line.hat_str() + line.name) + " " + event->serv->get_nick() + " " +
				(line.is_action()? "*" : ":") + line.trimmed(line.message), "msg");
		});


		pingpong::events::listen<pingpong::quit_event>([&](pingpong::quit_event *event) {
			log(event->who, event->who->name + " :" + event->content, "quit");

			// If you quit, close all logs associated with the server you quit from.
			if (event->who->is_self()) {
				std::vector<log_pair> to_close;
				to_close.reserve(filemap.size());
				for (const auto &pair: filemap) {
					if (pair.first.first == event->serv)
						to_close.push_back(pair.first);
				}

				for (const log_pair &pair: to_close)
					close(pair);
			}
		});


		pingpong::events::listen<pingpong::topic_event>([&](pingpong::topic_event *event) {
			log({event->serv, event->chan->name}, event->who->name + " :" + event->content, "topic_set");
		});


		pingpong::events::listen<pingpong::topic_updated_event>([&](pingpong::topic_updated_event *event) {
			log({event->serv, event->chan->name}, ":" + event->content, "topic_is");
		});
	}
}

spjalla::plugins::logs::logs_plugin ext_plugin {};
