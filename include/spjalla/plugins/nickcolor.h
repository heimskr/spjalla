#ifndef SPJALLA_PLUGINS_NICKCOLOR_H_
#define SPJALLA_PLUGINS_NICKCOLOR_H_

#include <unordered_map>

#include "spjalla/plugins/plugin.h"

namespace spjalla::plugins {
	class nickcolor_plugin: public plugin {
		private:
			std::unordered_map<client *, std::vector<int>> colorlists {};

		public:
			std::string get_name()        const override { return "Nick Colorizer"; }
			std::string get_description() const override { return "Colors nicks."; }
			std::string get_version()     const override { return "0.1.0"; }
			void preinit(plugin_host *)  override;
			void postinit(plugin_host *) override;

			static config::validation_result validate_colorlist(const config::value &);
			static void apply_colorlist(config::database &, const config::value &);
	};
}

#endif
