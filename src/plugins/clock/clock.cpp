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

			virtual ~clock_widget() = default;

			const char * get_name() const override { return "Clock"; }

			std::string _render(const ui::window *, bool) const override {
				std::chrono::system_clock::time_point tpoint {pingpong::util::timetype(stamp)};
				std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
				char str[64];
				std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
				return str;
			}

			void update() override {
				stamp = pingpong::util::timestamp();
				parent->render_statusbar();
			}
	};

	class clock_widget_plugin: public plugin {
		private:
			std::shared_ptr<clock_widget> widget;
			size_t ticks;
			client::heartbeat_listener tick_listener = std::make_shared<std::function<void(int)>>([this](int period) {
				tick(period);
			});

		public:
			virtual ~clock_widget_plugin() = default;

			std::string get_name()        const override { return "Clock"; }
			std::string get_description() const override { return "Shows a clock in the status bar."; }
			std::string get_version()     const override { return "0.1.0"; }

			void postinit(plugin_host *host) override {
				parent = dynamic_cast<spjalla::client *>(host);
				if (!parent) {
					DBG("Error: expected client as plugin host");
					return;
				}

				widget = std::make_shared<clock_widget>(parent, 0);
				parent->add_status_widget(widget);
				parent->add_heartbeat_listener(tick_listener);
			}

			void cleanup(plugin_host *) override {
				parent->remove_status_widget(widget);
				parent->remove_heartbeat_listener(tick_listener);
			}

			void tick(int period) {
				if (++ticks % (pingpong::util::precision / period) == 0 && widget)
					widget->update();
			}
	};
}

spjalla::plugins::clock_widget_plugin ext_plugin {};
