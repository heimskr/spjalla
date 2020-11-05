#include "pingpong/core/Util.h"
#include "pingpong/events/NickUpdated.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"
#include "spjalla/plugins/Plugin.h"
#include "spjalla/ui/StatusWidget.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	class WindowInfoLeftWidget: public Spjalla::UI::StatusWidget {
		private:
			long stamp = PingPong::Util::timestamp();

		protected:
			std::string _render(const UI::Window *win, bool) const override {
				if (!win)
					return "null?";

				std::string index = ansi::dim(ansi::bold(std::to_string(win->getIndex() + 1))) + " ";

				if (win->isStatus()) {
					return index + "status";
				} else if (win->isChannel()) {
					return index + Util::colorizeIfDead(win->channel->name, win);
				} else if (win->isUser()) {
					return index + Util::colorizeIfDead(win->user->name, win);
				} else {
					return ansi::bold(win->windowName);
				}
			}

		public:
			using StatusWidget::StatusWidget;

			virtual ~WindowInfoLeftWidget() {}

			const char * getName() const override { return "Window Info (left)"; }
	};

	class WindowInfoRightWidget: public Spjalla::UI::StatusWidget {
		private:
			long stamp = PingPong::Util::timestamp();

		protected:
			std::string _render(const UI::Window *win, bool) const override {
				if (!win) {
					return "null?";
				} else if (win->isStatus() || win->isChannel() || win->isUser()) {
					const std::string id = win->server? win->server->id : parent->activeServerID();
					return id.empty()? "none"_i : id;
					return parent->activeServerID();
				} else {
					return "~";
				}
			}

		public:
			using StatusWidget::StatusWidget;

			virtual ~WindowInfoRightWidget() {}

			const char * getName() const override { return "Window Info (right)"; }
	};

	class WindowInfoWidgetPlugin: public Plugin {
		private:
			std::shared_ptr<WindowInfoLeftWidget>  widgetLeft;
			std::shared_ptr<WindowInfoRightWidget> widgetRight;

		public:
			virtual ~WindowInfoWidgetPlugin() {}

			std::string getName()        const override { return "Window Name"; }
			std::string getDescription() const override { return "Shows the name of the current window in the status"
				" bar."; }
			std::string getVersion()     const override { return "0.1.0"; }

			void postinit(PluginHost *host) override {
				parent = dynamic_cast<Spjalla::Client *>(host);
				if (!parent) { DBG("Error: expected client as plugin host"); return; }

				widgetLeft  = std::make_shared<WindowInfoLeftWidget> (parent, 10);
				widgetRight = std::make_shared<WindowInfoRightWidget>(parent, 15);
				parent->addStatusWidget(widgetLeft);
				parent->addStatusWidget(widgetRight);

				PingPong::Events::listen<PingPong::NickUpdatedEvent>("p:window_info",
					[&, this](PingPong::NickUpdatedEvent *ev) {
						// If this user's window is open, redraw the statusbar to update the widget.
						if (parent->getUI().getActiveUser() == ev->who)
							parent->getUI().updateStatusbar();
					});
			}

			void cleanup(PluginHost *) override {
				PingPong::Events::unlisten<PingPong::NickUpdatedEvent>("p:window_info");
				parent->removeStatusWidget(widgetLeft);
				parent->removeStatusWidget(widgetRight);
			}
	};
}

Spjalla::Plugins::WindowInfoWidgetPlugin ext_plugin {};
