#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "haunted/core/Key.h"

#include "pingpong/events/Join.h"
#include "pingpong/events/Kick.h"
#include "pingpong/events/Mode.h"
#include "pingpong/events/Nick.h"
#include "pingpong/events/Notice.h"
#include "pingpong/events/Part.h"
#include "pingpong/events/Privmsg.h"
#include "pingpong/events/Quit.h"
#include "pingpong/events/Topic.h"
#include "pingpong/events/TopicUpdated.h"

#include "spjalla/config/Config.h"
#include "spjalla/config/Defaults.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/core/PluginHost.h"
#include "spjalla/core/Util.h"

#include "spjalla/lines/Notice.h"
#include "spjalla/lines/Privmsg.h"

#include "spjalla/plugins/Plugin.h"
#include "spjalla/plugins/Logs.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins::Logs {
	LogsPlugin::~LogsPlugin() {
		while (!filemap.empty())
			close(filemap.begin()->first);
	}

	void LogsPlugin::log(const LogPair &pair, const std::string &message, const std::string &type) {
		(getStream(pair) << PingPong::Util::timestamp() << precisionSuffix() << " " << type << " " << message << "\n").flush();
	}

	void LogsPlugin::log(std::shared_ptr<PingPong::User> user, const std::string &message, const std::string &type) {
		for (UI::Window *window: parent->getUI().windowsForUser(user)) {
			switch (window->type) {
				case UI::WindowType::Channel:
					log({user->server, window->channel->name}, message, type);
					break;
				case UI::WindowType::User:
					log({user->server, window->user->name}, message, type);
					close({user->server, window->user->name});
					break;
				default: ;
			}
		}
	}

	void LogsPlugin::log(PingPong::LocalEvent *event) {
		if (event->server->getParent() == &parent->getIRC())
			log({event->server, event->where}, *event);
	}

	std::fstream & LogsPlugin::getStream(const LogPair &pair) {
		if (filemap.count(pair) == 1)
			return filemap.at(pair);

		const std::filesystem::path path = getPath(pair);
		const bool existed = std::filesystem::exists(path);
		std::fstream new_stream(path, std::ios::app | std::ios::in | std::ios::out);
		if (!new_stream)
			throw std::runtime_error("Couldn't open file stream for " + std::string(path));
		if (!existed)
			new_stream << PingPong::Util::timestamp() << precisionSuffix() << " created" << "\n";
		(new_stream << PingPong::Util::timestamp() << precisionSuffix() << " opened" << "\n").flush();
		std::vector<std::string> lines;
		filemap.insert({pair, std::move(new_stream)});
		return filemap.at(pair);
	}

	bool LogsPlugin::close(const LogPair &pair) {
		if (filemap.count(pair) == 0)
			return false;

		std::fstream &stream = getStream(pair);
		(stream << PingPong::Util::timestamp() << precisionSuffix() << " closed\n").flush();
		stream.close();
		filemap.erase(pair);
		return true;
	}

	std::filesystem::path LogsPlugin::getPath(const LogPair &pair) {
		const std::filesystem::path dir = base / sanitizeFilename(std::string(pair.first->id));
		if (!std::filesystem::exists(dir))
			std::filesystem::create_directories(dir);

		return dir / sanitizeFilename(pair.second);
	}

	std::string & LogsPlugin::sanitizeFilename(std::string &str) {
		const static std::string invalid = "\x07/ \\,.:*\0";
		for (auto iter = str.begin(); iter != str.end(); ++iter) {
			if (invalid.find(*iter) != std::string::npos)
				*iter = '_';
		}
		return str;
	}

	std::string LogsPlugin::sanitizeFilename(const std::string &str) {
		std::string out {str};
		sanitizeFilename(out);
		return out;
	}

	constexpr char LogsPlugin::precisionSuffix() {
		switch (PingPong::Util::precision) {
			case 1'000'000'000L: return 'n';
			case 1'000'000L: return 'u';
			case 1'000L: return 'm';
			default:  return 's';
		}
	}

	std::chrono::microseconds LogsPlugin::parseStamp(std::string stampStr) {
		if (stampStr.empty())
			throw std::invalid_argument("Timestamp is empty");

		char suffix = 's';
		const static std::string suffixes = "nums";
		if (suffixes.find(stampStr.back()) != std::string::npos) {
			suffix = stampStr.back();
			stampStr.pop_back();
		}

		long parsed;
		if (!formicine::util::parse_long(stampStr, parsed))
			throw std::invalid_argument("Invalid timestamp: " + stampStr);

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
		
	void LogsPlugin::preinit(PluginHost *host) {
		Spjalla::Client *client = dynamic_cast<Spjalla::Client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

		Config::RegisterKey("logs", "autoclean", true, Config::validateBool, {},
			"Whether /restore should exclude lines that indicate when the log was opened, created or closed.");
		Config::RegisterKey("logs", "default_restore", 128, Config::validateLong, {},
			"The default number of scrollback lines to restore with /restore.");
		Config::RegisterKey("logs", "enabled", true, Config::validateBool, {}, "Whether to enable logs.");

		base = Util::getHome() / DEFAULT_DATA_DIR / "logs";
		if (!std::filesystem::exists(base)) {
			DBG("Created log directory.");
			std::filesystem::create_directories(base);
		} else if (!std::filesystem::is_directory(base)) {
			throw std::runtime_error("A file already exists at " + std::string(base));
		}
	}

	void LogsPlugin::postinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		parent->add("clean", 0, 1, true, [this](PingPong::Server *, const InputLine &) {
			clean();
		});

		parent->add("restore", 0, 1, true, [this](PingPong::Server *server, const InputLine &il) {
			restore(server, il);
		});

		PingPong::Events::listen<PingPong::JoinEvent>("p:logs", [&](PingPong::JoinEvent *event) {
			log({event->server, event->channel->name}, event->who->name, "join");
		});


		PingPong::Events::listen<PingPong::KickEvent>("p:logs", [&](PingPong::KickEvent *event) {
			log({event->server, event->channel->name}, event->who->name + " " + event->whom->name + " :" +
				event->content, "kick");
		});


		PingPong::Events::listen<PingPong::ModeEvent>("p:logs", [&](PingPong::ModeEvent *event) {
			// Ignore self mode changes.
			if (event->getName() == event->where)
				return;

			const std::string extra = !event->modeSet.extra.empty()? " " + event->modeSet.extra : "";
			log({event->server, event->where}, event->getName() + " " + event->server->getNick() + " " +
				event->modeSet.modeString() + extra, "mode");
		});


		PingPong::Events::listen<PingPong::NickEvent>("p:logs", [&](PingPong::NickEvent *event) {
			log(event->who, event->content + " " + event->who->name, "nick");
		});


		PingPong::Events::listen<PingPong::NoticeEvent>("p:logs", [&, this](PingPong::NoticeEvent *event) {
			if (event->server->getParent() != &parent->getIRC())
				return;

			Lines::NoticeLine line = event->isChannel() || event->speaker? Lines::NoticeLine(parent, *event) :
				Lines::NoticeLine(parent, event->server->id, "*", event->server->getNick(), event->content,
					event->stamp, {}, true);

			// A '$' logfile is for messages received from the server itself.
			const std::string where = event->isChannel()? event->where : (event->speaker? event->speaker->name : "$");
			std::string nick = event->server->getNick();
			if (nick.empty())
				nick = "!"; // A '!', in this case, represents you before your nick has been set.

			log({event->server, where}, formicine::util::ltrim(line.hatString() + line.name) + " " + nick + " " +
				(line.isAction()? "*" : ":") + line.trimmed(line.message), "notice");
		});


		PingPong::Events::listen<PingPong::PartEvent>("p:logs", [&](PingPong::PartEvent *event) {
			log({event->server, event->channel->name}, event->who->name + " :" + event->content, "part");

			// If you parted the channel, close the channel's stream.
			if (event->who->isSelf())
				close({event->server, event->channel->name});
		});


		PingPong::Events::listen<PingPong::PrivmsgEvent>("p:logs", [&, this](PingPong::PrivmsgEvent *event) {
			if (event->server->getParent() != &parent->getIRC()) {
				DBG("Different parent IRCs: " << event->server->getParent() << " vs. " << &parent->getIRC());
				return;
			}

			Lines::PrivmsgLine line {parent, *event};
			log({event->server, event->isChannel()? event->where : event->speaker->name},
				formicine::util::ltrim(line.hatString() + line.name) + " " + event->server->getNick() + " " +
				(line.isAction()? "*" : ":") + line.trimmed(line.message), "msg");
		});


		PingPong::Events::listen<PingPong::QuitEvent>("p:logs", [&](PingPong::QuitEvent *event) {
			log(event->who, event->who->name + " :" + event->content, "quit");

			// If you quit, close all logs associated with the server you quit from.
			if (event->who->isSelf()) {
				std::vector<LogPair> to_close;
				to_close.reserve(filemap.size());
				for (const auto &pair: filemap) {
					if (pair.first.first == event->server)
						to_close.push_back(pair.first);
				}

				for (const LogPair &pair: to_close)
					close(pair);
			}
		});


		PingPong::Events::listen<PingPong::TopicEvent>("p:logs", [&](PingPong::TopicEvent *event) {
			log({event->server, event->channel->name}, event->who->name + " :" + event->content, "topic_set");
		});


		PingPong::Events::listen<PingPong::TopicUpdatedEvent>("p:logs", [&](PingPong::TopicUpdatedEvent *event) {
			log({event->server, event->channel->name}, ":" + event->content, "topic_is");
		});
	}

	void LogsPlugin::cleanup(PluginHost *) {
		Config::unregister("logs", "autoclean");
		Config::unregister("logs", "default_restore");
		Config::unregister("logs", "enabled");

		PingPong::Events::unlisten<PingPong::JoinEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::KickEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::ModeEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::NickEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::NoticeEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::PartEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::PrivmsgEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::QuitEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::TopicEvent>("p:logs");
		PingPong::Events::unlisten<PingPong::TopicUpdatedEvent>("p:logs");

		parent->removeCommand("clean");
		parent->removeCommand("restore");
	}
}

Spjalla::Plugins::Logs::LogsPlugin ext_plugin {};
