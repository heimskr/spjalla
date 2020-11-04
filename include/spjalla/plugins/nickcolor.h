#ifndef SPJALLA_PLUGINS_NICKCOLOR_H_
#define SPJALLA_PLUGINS_NICKCOLOR_H_

#include <unordered_map>

#include "spjalla/plugins/Plugin.h"

namespace Spjalla::Plugins {
	class NickColorPlugin: public plugin {
		private:
			std::vector<ansi::color> colorlist {};

		public:
			std::string getName()        const override { return "Nick Colorizer"; }
			std::string getDescription() const override { return "Colors nicks."; }
			std::string getVersion()     const override { return "0.1.0"; }
			void preinit(PluginHost  *) override;
			void postinit(PluginHost *) override;
			void cleanup(PluginHost  *) override;

			static Config::ValidationResult validateColorlist(const Config::Value &);
	};
}

#endif
