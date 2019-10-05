#include <fstream>
#include <map>
#include <utility>

#include "haunted/core/key.h"

#include "pingpong/events/privmsg.h"

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

			void log(const log_pair &, const std::string &message);
			void log(const log_pair &, const std::string &message, long stamp);

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
		for (const auto &pair: filemap) {
			filemap.find(pair.first)->second.close();
		}
	}

	void logs_plugin::log(const log_pair &pair, const std::string &message) {
		DBG("log: pair[" << pair.first->id << "/" << pair.second << "] message" << "{"_d << ansi::bold(message) << "}"_d);
		(get_stream(pair) << "%_ " << pingpong::util::millistamp() << " " << message << "\n").flush();
	}

	void logs_plugin::log(const log_pair &pair, const std::string &message, long stamp) {
		log(pair, lines::render_time(stamp, false) + " " + message);
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
			new_stream << "%created " << pingpong::util::millistamp() << "\n";
		(new_stream << "%opened " << pingpong::util::millistamp() << "\n").flush();
		filemap.insert({pair, std::move(new_stream)});
		return filemap.at(pair);
	}

	std::filesystem::path logs_plugin::get_path(const log_pair &pair) {
		const std::filesystem::path dir = base / sanitize_filename(pair.first->id);
		if (!std::filesystem::exists(dir)) {
			DBG("Creating directories for " << dir);
			std::filesystem::create_directories(dir);
		}

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

		pingpong::events::listen<pingpong::privmsg_event>([&](pingpong::privmsg_event *event) {
			lines::privmsg_line line {*event};
			log({event->serv, event->where}, line.hat_str() + line.name + " " + (line.is_action()? ":" : "*") +
				line.trimmed(line.message));
		});
	}
}

spjalla::plugins::logs_plugin ext_plugin {};
