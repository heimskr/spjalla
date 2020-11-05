#ifndef SPJALLA_PLUGINS_NOTIFICATIONS_H_
#define SPJALLA_PLUGINS_NOTIFICATIONS_H_

#include "spjalla/plugins/Plugin.h"
#include "spjalla/lines/Line.h"

namespace Spjalla::Plugins {
	class NotificationsWidget;

	class NotificationsPlugin: public Plugin {
		private:
			std::shared_ptr<NotificationsWidget> widget;
			long gatheringSince = -1;
			std::vector<std::shared_ptr<Lines::Line>> gatheredLines {};

		public:
			virtual ~NotificationsPlugin() {}

			std::string getName()        const override { return "Notifications"; }
			std::string getDescription() const override { return "Shows a notifications widget in the status bar."; }
			std::string getVersion()     const override { return "0.1.1"; }

			bool gathering() const;
			void startGathering();
			void stopGathering();

			void preinit(PluginHost  *host) override;
			void postinit(PluginHost *host) override;
			void cleanup(PluginHost  *host) override;
	};
}

#endif
