#ifndef SPJALLA_PLUGINS_LOGS_H_
#define SPJALLA_PLUGINS_LOGS_H_

#include "pingpong/core/server.h"
#include "pingpong/events/event.h"
#include "spjalla/plugins/plugin.h"

namespace spjalla::plugins {
	class logs_plugin: public plugin {
		public:
			using log_pair = std::pair<pingpong::server *, std::string>;

		private:
			/** Maps server-channel pairs to the filestreams where their logs are stored. */
			std::map<log_pair, std::fstream> filemap;

			std::filesystem::path base;

		public:
			~logs_plugin();

			std::string get_name()        const override { return "Logger"; }
			std::string get_description() const override { return "Logs messages."; }
			std::string get_version()     const override { return "0.1.0"; }
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
			std::fstream & get_stream(const log_pair &);

			/** Closes and removes the output stream corresponding to a location. Returns true if one existed. */
			bool close(const log_pair &);

			std::filesystem::path get_path(const log_pair &);

			static std::string & sanitize_filename(std::string &);
			static std::string sanitize_filename(const std::string &);

			/** Converts a line of log text into a textline for a window. */
			static std::unique_ptr<lines::line> get_line(const std::string &);
	};
}

#endif
