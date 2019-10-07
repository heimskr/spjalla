#include "pingpong/core/util.h"

#include "spjalla/core/client.h"
#include "spjalla/plugins/plugin.h"
#include "spjalla/ui/status_widget.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	class clock_widget: public spjalla::ui::status_widget {
		private:
			long stamp = pingpong::util::timestamp();

		public:
			using status_widget::status_widget;

			virtual ~clock_widget() {}

			std::string render(const ui::window *, bool) const {
				std::chrono::system_clock::time_point tpoint {pingpong::util::timetype(stamp)};
				std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
				char str[64];
				std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
				return str;
			}

			void update() {
				stamp = pingpong::util::timestamp();
				parent->render_statusbar();
			}
	};

	class clock_widget_plugin: public plugin {
		private:
			std::shared_ptr<clock_widget> widget;
			size_t ticks;

		public:
			virtual ~clock_widget_plugin() {}

			std::string get_name()        const override { return "Clock"; }
			std::string get_description() const override { return "Shows a clock in the status bar."; }
			std::string get_version()     const override { return "0.1.0"; }

			void postinit(plugin_host *host) override {
				spjalla::client *client = dynamic_cast<spjalla::client *>(host);
				if (!client) {
					DBG("Error: expected client as plugin host");
					return;
				}

				widget = std::make_shared<clock_widget>(client, 0);
				client->add_status_widget(widget);
				client->add_heartbeat_listener([this](int period) { tick(period); });
			}

			void tick(int period) {
				if (++ticks % (pingpong::util::precision / period) == 0 && widget)
					widget->update();
			}
	};
}

spjalla::plugins::clock_widget_plugin ext_plugin {};
