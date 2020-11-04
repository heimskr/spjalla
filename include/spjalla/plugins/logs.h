#ifndef SPJALLA_PLUGINS_LOGS_H_
#define SPJALLA_PLUGINS_LOGS_H_

#include <chrono>

#include "pingpong/core/Server.h"
#include "pingpong/events/Event.h"
#include "spjalla/plugins/Plugin.h"
#include "spjalla/lines/Line.h"

namespace Spjalla::Plugins::Logs {
	class LogsPlugin: public Plugin {
		public:
			using LogPair = std::pair<PingPong::server *, std::string>;

		private:
			/** Maps server-channel pairs to the filestreams where their logs are stored. */
			std::map<LogPair, std::fstream> filemap;

			std::filesystem::path base;

		public:
			~LogsPlugin();

			std::string getName()        const override { return "Logger"; }
			std::string getDescription() const override { return "Logs messages."; }
			std::string getVersion()     const override { return "0.1.0"; }
			void preinit(PluginHost  *) override;
			void postinit(PluginHost *) override;
			void cleanup(PluginHost  *) override;

			/** Logs a message of a given type ("_" by default) to a single location. */
			void log(const LogPair &, const std::string &message, const std::string &type = "_");

			/** Logs a message in all locations where a user is present. */
			void log(std::shared_ptr<PingPong::User>, const std::string &message, const std::string &type = "_");

			template <typename T>
			void log(const LogPair &pair, const T &anything) {
				log(pair, std::to_string(anything));
			}

			void log(PingPong::LocalEvent *);

			/** Returns the output stream corresponding to a location, creating one if necessary. */
			std::fstream & getStream(const LogPair &);

			/** Closes and removes the output stream corresponding to a location. Returns true if one existed. */
			bool close(const LogPair &);

			std::filesystem::path getPath(const LogPair &);

			static std::string & sanitizeFilename(std::string &);
			static std::string sanitizeFilename(const std::string &);

			void clean();
			void restore(PingPong::server *serv, const InputLine &il);

			/** Converts a line of log text into a textline for a window. */
			std::unique_ptr<lines::line> getLine(const LogPair &, const std::string &, bool autoclean = false);

			static constexpr char precisionSuffix();
			static std::chrono::microseconds parseStamp(std::string);
	};
}

#endif
