#ifndef SPJALLA_PLUGINS_NOTIFICATIONS_H_
#define SPJALLA_PLUGINS_NOTIFICATIONS_H_

#include "spjalla/plugins/plugin.h"
#include "spjalla/lines/line.h"

namespace spjalla::plugins {
	class notifications_widget;

	class notifications_plugin: public plugin {
		private:
			std::shared_ptr<notifications_widget> widget;
			long gathering_since = -1;
			std::vector<std::shared_ptr<lines::line>> gathered_lines {};

		public:
			virtual ~notifications_plugin() {}

			std::string get_name()        const override { return "Notifications"; }
			std::string get_description() const override { return "Shows a notifications widget in the status bar."; }
			std::string get_version()     const override { return "0.1.1"; }

			bool gathering() const;
			void start_gathering();
			void stop_gathering();

			void preinit(plugin_host  *host) override;
			void postinit(plugin_host *host) override;
			void cleanup(plugin_host  *host) override;
	};
}

#endif
