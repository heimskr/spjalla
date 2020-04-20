#ifndef SPJALLA_PLUGINS_TRIGGER_H_
#define SPJALLA_PLUGINS_TRIGGER_H_

#include "spjalla/plugins/plugin.h"

namespace spjalla::plugins {
	class trigger_plugin: public plugin {
		private:
			bool active = false;
			std::string reason;

		public:
			virtual ~trigger_plugin() {}

			std::string get_name()        const override { return "Trigger"; }
			std::string get_description() const override { return "Assists in quick draw op battles."; }
			std::string get_version()     const override { return "0.0.1"; }

			void preinit(plugin_host  *host) override;
			void postinit(plugin_host *host) override;
			void cleanup(plugin_host  *host) override;
	};
}

#endif
