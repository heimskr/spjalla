// TODO: configurable timestamp precision

#include <fstream>
#include <map>
#include <utility>

#include "haunted/core/key.h"

#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/mode.h"
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

#include "spjalla/lines/privmsg.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	class logs_plugin: public plugin {
		public:
			using log_pair = std::pair<pingpong::server *, std::string>;

		private:
			/** Maps server-channel pairs to the filestreams where their logs are stored. */
			std::map<log_pair, std::ofstream> filemap;

			std::filesystem::path base;

		public:
			~logs_plugin();

			std::string get_name()        const override { return "Logger"; }
			std::string get_description() const override { return "Logs messages."; }
			std::string get_version()     const override { return "0.0.2"; }
			void preinit(plugin_host *) override;
			void postinit(plugin_host *) override;

			/** Logs a message of a given type ("_" by default) to a single location. */
			void log(const log_pair &, const std::string &message, const std::string &type = "_");

			/** Logs a message in all locations where a user is present. */
			void log(std::shared_ptr<pingpong::user>, const std::string &message, const std::string &type = "_");

			template <typename T>
			void log(const log_pair &pair, const T &anything) {
				log(pair, std::to_string(anything));
			}

			void log(pingpong::local_event *);

			/** Returns the output stream corresponding to a location, creating one if necessary. */
			std::ofstream & get_stream(const log_pair &);

			/** Closes and removes the output stream corresponding to a location. Returns true if one existed. */
			bool close(const log_pair &);

			std::filesystem::path get_path(const log_pair &);

			static std::string & sanitize_filename(std::string &);
			static std::string sanitize_filename(const std::string &);
	};

	logs_plugin::~logs_plugin() {
		while (!filemap.empty())
			close(filemap.begin()->first);
	}

	void logs_plugin::log(const log_pair &pair, const std::string &message, const std::string &type) {
		(get_stream(pair) << pingpong::util::millistamp() << " " << type << " " << message << "\n").flush();
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

	std::ofstream & logs_plugin::get_stream(const log_pair &pair) {
		if (filemap.count(pair) == 1)
			return filemap.at(pair);

		const std::filesystem::path path = get_path(pair);
		const bool existed = std::filesystem::exists(path);
		std::ofstream new_stream(path, std::ios::app);
		if (!new_stream)
			throw std::runtime_error("Couldn't open file stream for " + std::string(path));
		if (!existed)
			new_stream << pingpong::util::millistamp() << " created" << "\n";
		(new_stream << pingpong::util::millistamp() << " opened" << "\n").flush();
		filemap.insert({pair, std::move(new_stream)});
		return filemap.at(pair);
	}

	bool logs_plugin::close(const log_pair &pair) {
		if (filemap.count(pair) == 0)
			return false;

		std::ofstream &stream = get_stream(pair);
		(stream << pingpong::util::millistamp() << " closed\n").flush();
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
		
	void logs_plugin::preinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }
		parent = client;

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
			log({event->serv, event->where}, event->get_name() + " " + event->mset.mode_str() + extra, "mode");
		});


		pingpong::events::listen<pingpong::privmsg_event>([&](pingpong::privmsg_event *event) {
			lines::privmsg_line line {*event};
			log({event->serv, event->is_channel()? event->where : event->speaker->name},
				line.hat_str() + line.name + " " + (line.is_action()? "*" : ":") + line.trimmed(line.message), "msg");
		});


		pingpong::events::listen<pingpong::part_event>([&](pingpong::part_event *event) {
			log({event->serv, event->chan->name}, event->who->name + " :" + event->content, "part");

			// If you parted the channel, close the channel's stream.
			if (event->who->is_self())
				close({event->serv, event->chan->name});
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

spjalla::plugins::logs_plugin ext_plugin {};
