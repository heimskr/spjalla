#ifndef SPJALLA_PLUGINS_TRIGGER_H_
#define SPJALLA_PLUGINS_TRIGGER_H_

#include "spjalla/plugins/Plugin.h"

namespace Spjalla::Plugins {
	class TriggerPlugin: public Plugin {
		private:
			bool active = false;
			std::string reason;

		public:
			virtual ~TriggerPlugin() {}

			std::string getName()        const override { return "Trigger"; }
			std::string getDescription() const override { return "Assists in quick draw op battles."; }
			std::string getVersion()     const override { return "0.0.1"; }

			void preinit(PluginHost  *) override;
			void postinit(PluginHost *) override;
			void cleanup(PluginHost  *) override;
	};
}

#endif
