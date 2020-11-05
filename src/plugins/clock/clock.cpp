#include "pingpong/core/Util.h"

#include "spjalla/core/Client.h"
#include "spjalla/plugins/Plugin.h"
#include "spjalla/ui/StatusWidget.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	class ClockWidget: public Spjalla::UI::StatusWidget {
		private:
			long stamp = PingPong::Util::timestamp();

		public:
			using StatusWidget::StatusWidget;

			virtual ~ClockWidget() = default;

			const char * getName() const override { return "Clock"; }

			std::string _render(const UI::Window *, bool) const override {
				std::chrono::system_clock::time_point tpoint {PingPong::Util::TimeType(stamp)};
				std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
				char str[64];
				std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
				return str;
			}

			void update() override {
				stamp = PingPong::Util::timestamp();
				parent->renderStatusbar();
			}
	};

	class ClockWidgetPlugin: public Plugin {
		private:
			std::shared_ptr<ClockWidget> widget;
			size_t ticks;
			Client::HeartbeatListener tickListener = std::make_shared<std::function<void(int)>>([this](int period) {
				tick(period);
			});

		public:
			virtual ~ClockWidgetPlugin() = default;

			std::string getName()        const override { return "Clock"; }
			std::string getDescription() const override { return "Shows a clock in the status bar."; }
			std::string getVersion()     const override { return "0.1.0"; }

			void postinit(PluginHost *host) override {
				parent = dynamic_cast<Spjalla::Client *>(host);
				if (!parent) {
					DBG("Error: expected client as plugin host");
					return;
				}

				widget = std::make_shared<ClockWidget>(parent, 0);
				parent->addStatusWidget(widget);
				parent->addHeartbeatListener(tickListener);
			}

			void cleanup(PluginHost *) override {
				parent->removeStatusWidget(widget);
				parent->removeHeartbeatListener(tickListener);
			}

			void tick(int period) {
				if (++ticks % (PingPong::Util::precision / period) == 0 && widget)
					widget->update();
			}
	};
}

Spjalla::Plugins::ClockWidgetPlugin ext_plugin {};
