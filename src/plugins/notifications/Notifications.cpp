#include <unordered_map>

#include "spjalla/plugins/Notifications.h"
#include "spjalla/plugins/notifications/Widget.h"

#include "pingpong/core/Util.h"
#include "pingpong/events/Event.h"
#include "spjalla/core/Client.h"
#include "spjalla/config/Defaults.h"
#include "spjalla/events/Notification.h"
#include "spjalla/events/WindowChanged.h"
#include "spjalla/lines/Basic.h"
#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins {
	bool NotificationsPlugin::gathering() const {
		return gatheringSince != -1;
	}

	void NotificationsPlugin::startGathering() {
		gatheringSince = PingPong::Util::seconds();
		gatheredLines = {};
		parent->getUI().log("Gathering notifications.");
	}

	void NotificationsPlugin::stopGathering() {
		if (!gathering())
			return;

		const std::string range = ansi::bold(PingPong::Util::formatTime(PingPong::Util::fromSeconds(gatheringSince),
			"%Y/%m/%d %H:%M:%S")) + " and " + ansi::bold(PingPong::Util::formatTime(PingPong::Util::now(),
			"%Y/%m/%d %H:%M:%S"));

		if (gatheredLines.empty()) {
			parent->getUI().warn("No notifications gathered between " + range + ".");
		} else {
			UI::Window *win = parent->getUI().getWindow("gather", true, UI::WindowType::Other);
			parent->getUI().focusWindow(win);
			win->clearLines();
			win->add<Lines::BasicLine>(parent, "Notifications between " + range + ":");
			for (std::shared_ptr<Lines::Line> &line: gatheredLines) {
				// Note: since lines::line isn't constructible, we can't create a copy and set its box to this window.
				// We have to share the old line with the window it was in if it had one.
				if (!line->box)
					line->box = win;
				*win += line;
			}

			gatheredLines.clear();
		}

		gatheringSince = -1;
	}

	void NotificationsPlugin::preinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		widget = std::make_shared<NotificationsWidget>(parent, 20);

		Config::RegisterKey("appearance", "highlight_color", "red", Config::validateColor,
			[&, this](Config::Database &, const Config::Value &value) {
				widget->highlightColor = ansi::get_color(value.string_());
				parent->renderStatusbar();
			}, "The text color for highlight notifications.");

		Config::RegisterKey("appearance", "highlight_bold", true, Config::validateBool,
			[&, this](Config::Database &, const Config::Value &value) {
				widget->highlightBold = value.bool_();
				parent->renderStatusbar();
			}, "Whether to render highlight notifications in bold.");
	}

	void NotificationsPlugin::postinit(PluginHost *) {
		parent->addStatusWidget(widget);

		parent->add("gather", 0, 0, false, [&](PingPong::Server *, const InputLine &) {
			if (gathering())
				stopGathering();
			else
				startGathering();
		});

		PingPong::Events::listen<Events::NotificationEvent>("p:notifications",
			[=, this](Events::NotificationEvent *ev) {
				if (gathering() && ev->line->getNotificationType() == NotificationType::Highlight)
					gatheredLines.push_back(ev->line);
			});

		PingPong::Events::listen<Events::WindowChangedEvent>("p:notifications",
			[=, this](Events::WindowChangedEvent *ev) {
				widget->windowFocused(ev->to);
			});

		PingPong::Events::listen<Events::WindowNotificationEvent>("p:notifications",
			[=, this](Events::WindowNotificationEvent *ev) {
				if (ev->window == parent->getUI().getActiveWindow())
					ev->window->highestNotification = NotificationType::None;
				else
					parent->renderStatusbar();
			});
	}

	void NotificationsPlugin::cleanup(PluginHost *) {
		Config::unregister("appearance", "highlight_color");
		Config::unregister("appearance", "highlight_bold");

		PingPong::Events::unlisten<Events::NotificationEvent>("p:notifications");
		PingPong::Events::unlisten<Events::WindowChangedEvent>("p:notifications");
		PingPong::Events::unlisten<Events::WindowNotificationEvent>("p:notifications");
		parent->removeStatusWidget(widget);
	}
}

Spjalla::Plugins::NotificationsPlugin ext_plugin {};
