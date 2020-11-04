#include <unordered_map>

#include "spjalla/plugins/notifications.h"
#include "spjalla/plugins/notifications/widget.h"

#include "pingpong/core/Util.h"
#include "pingpong/events/Event.h"
#include "spjalla/core/Client.h"
#include "spjalla/config/Defaults.h"
#include "spjalla/events/notification.h"
#include "spjalla/events/window_changed.h"
#include "spjalla/lines/basic.h"
#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/futil.h"

namespace Spjalla::Plugins {
	bool notifications_plugin::gathering() const {
		return gathering_since != -1;
	}

	void notifications_plugin::start_gathering() {
		gathering_since = PingPong::Util::Seconds();
		gathered_lines = {};
		parent->getUI().log("Gathering notifications.");
	}

	void notifications_plugin::stop_gathering() {
		if (!gathering())
			return;

		const std::string range = ansi::bold(PingPong::Util::formatTime(PingPong::Util::fromSeconds(gathering_since),
			"%Y/%m/%d %H:%M:%S")) + " and " + ansi::bold(PingPong::Util::formatTime(PingPong::util::now(),
			"%Y/%m/%d %H:%M:%S"));

		if (gathered_lines.empty()) {
			parent->getUI().warn("No notifications gathered between " + range + ".");
		} else {
			UI::Window *win = parent->getUI().get_window("gather", true, UI::WindowType::other);
			parent->getUI().focus_window(win);
			win->clear_lines();
			win->add<lines::basic_line>(parent, "Notifications between " + range + ":");
			for (std::shared_ptr<lines::line> line: gathered_lines) {
				// Note: since lines::line isn't constructible, we can't create a copy and set its box to this window.
				// We have to share the old line with the window it was in if it had one.
				if (!line->box)
					line->box = win;
				*win += line;
			}

			gathered_lines.clear();
		}

		gathering_since = -1;
	}

	void notifications_plugin::preinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		widget = std::make_shared<notifications_widget>(parent, 20);

		config::RegisterKey("appearance", "highlight_color", "red", config::validate_color,
			[&, this](config::database &, const config::value &value) {
				widget->highlight_color = ansi::get_color(value.string_());
				parent->render_statusbar();
			}, "The text color for highlight notifications.");

		config::RegisterKey("appearance", "highlight_bold", true, config::validateBool,
			[&, this](config::database &, const config::value &value) {
				widget->highlight_bold = value.bool_();
				parent->render_statusbar();
			}, "Whether to render highlight notifications in bold.");
	}

	void notifications_plugin::postinit(PluginHost *) {
		parent->add_status_widget(widget);

		parent->add("gather", 0, 0, false, [&](PingPong::Server *, const InputLine &) {
			if (gathering())
				stop_gathering();
			else
				start_gathering();
		});

		PingPong::Events::listen<events::notification_event>("p:notifications",
			[=, this](events::notification_event *ev) {
				if (gathering() && ev->line->getNotificationType() == NotificationType::highlight)
					gathered_lines.push_back(ev->line);
			});

		PingPong::Events::listen<events::window_changed_event>("p:notifications",
			[=, this](events::window_changed_event *ev) {
				widget->window_focused(ev->to);
			});

		PingPong::Events::listen<events::window_notification_event>("p:notifications",
			[=, this](events::window_notification_event *ev) {
				if (ev->window == parent->getUI().get_active_window())
					ev->window->highest_notification = NotificationType::none;
				else
					parent->render_statusbar();
			});
	}

	void notifications_plugin::cleanup(PluginHost *) {
		config::unregister("appearance", "highlight_color");
		config::unregister("appearance", "highlight_bold");

		PingPong::Events::unlisten<events::notification_event>("p:notifications");
		PingPong::Events::unlisten<events::window_changed_event>("p:notifications");
		PingPong::Events::unlisten<events::window_notification_event>("p:notifications");
		parent->remove_status_widget(widget);
	}
}

spjalla::plugins::notifications_plugin ext_plugin {};
