#include <fstream>
#include <map>
#include <utility>

#include "haunted/core/key.h"

#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"
#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/plugin_host.h"
#include "spjalla/core/util.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	class logs_plugin: public plugin {
		public:
			using log_pair = std::pair<pingpong::server *, std::string>;

		private:
			/** Maps server-channel pairs to the filestreams where their logs are stored. */
			std::map<log_pair, std::fstream> filemap;

			std::filesystem::path base;

		public:
			~logs_plugin() {}

			std::string get_name()        const override { return "Logger"; }
			std::string get_description() const override { return "Logs messages."; }
			std::string get_version()     const override { return "0.0.1"; }
			void preinit(plugin_host *) override;

			void log(const log_pair &, const std::string &message);

			/** Returns the output stream corresponding to a location, creating one if necessary. */
			std::fstream & get_stream(const log_pair &);

			/** Closes and removes the output stream corresponding to a location. Returns true if one existed. */
			bool close(const log_pair &);

			std::filesystem::path get_path(const log_pair &);

			static std::string & sanitize_filename(std::string &);
			static std::string sanitize_filename(const std::string &);
	};

	void logs_plugin::log(const log_pair &pair, const std::string &message) {

	}

	std::fstream & logs_plugin::get_stream(const log_pair &pair) {
		if (filemap.count(pair) == 1)
			return filemap.at(pair);

		const std::string path = get_path(pair);
		std::fstream new_stream(path, std::ios::app);
		if (new_stream.peek() == new_stream.eof())
			new_stream << "[created " << pingpong::util::millistamp() << "]\n";
		else
			new_stream << "[opened " << pingpong::util::millistamp() << "]\n";
		filemap.insert({pair, std::move(new_stream)});
	}

	std::filesystem::path logs_plugin::get_path(const log_pair &pair) {
		return base / sanitize_filename(pair.first->id) / sanitize_filename(pair.second);
	}

	std::string & logs_plugin::sanitize_filename(std::string &str) {
		const static std::string invalid = "\x07/ \\,:*\0";
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
}

spjalla::plugins::logs_plugin ext_plugin {};
