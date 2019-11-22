#include <unordered_map>

#include "spjalla/plugins/notifications.h"
#include "spjalla/plugins/notifications/widget.h"

#include "pingpong/core/util.h"
#include "pingpong/events/event.h"
#include "spjalla/core/client.h"
#include "spjalla/config/defaults.h"
#include "spjalla/events/notification.h"
#include "spjalla/events/window_changed.h"
#include "spjalla/lines/basic.h"
#include "spjalla/plugins/plugin.h"

#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	bool notifications_plugin::gathering() const {
		return gathering_since != -1;
	}

	void notifications_plugin::start_gathering() {
		gathering_since = pingpong::util::seconds();
		gathered_lines = {};
		parent->get_ui().log("Gathering notifications.");
	}

	void notifications_plugin::stop_gathering() {
		if (!gathering())
			return;

		const std::string range = ansi::bold(pingpong::util::format_time(pingpong::util::from_seconds(gathering_since),
			"%Y/%m/%d %H:%M:%S")) + " and " + ansi::bold(pingpong::util::format_time(pingpong::util::now(),
			"%Y/%m/%d %H:%M:%S"));

		if (gathered_lines.empty()) {
			parent->get_ui().warn("No notifications gathered between " + range + ".");
		} else {
			ui::window *win = parent->get_ui().get_window("gather", true, ui::window_type::other);
			parent->get_ui().focus_window(win);
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

	void notifications_plugin::preinit(plugin_host *host) {
		parent = dynamic_cast<spjalla::client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		widget = std::make_shared<notifications_widget>(parent, 20);

		config::register_key("appearance", "highlight_color", "red", config::validate_color,
			[&, this](config::database &, const config::value &value) {
				widget->highlight_color = ansi::get_color(value.string_());
				parent->render_statusbar();
			}, "The text color for highlight notifications.");

		config::register_key("appearance", "highlight_bold", true, config::validate_bool,
			[&, this](config::database &, const config::value &value) {
				widget->highlight_bold = value.bool_();
				parent->render_statusbar();
			}, "Whether to render highlight notifications in bold.");
	}

	void notifications_plugin::postinit(plugin_host *) {
		parent->add_status_widget(widget);

		parent->add("gather", 0, 0, false, [&](pingpong::server *, const input_line &) {
			if (gathering())
				stop_gathering();
			else
				start_gathering();
		});

		pingpong::events::listen<events::notification_event>("p:notifications",
			[=, this](events::notification_event *ev) {
				if (gathering() && ev->line->get_notification_type() == notification_type::highlight)
					gathered_lines.push_back(ev->line);
			});

		pingpong::events::listen<events::window_changed_event>("p:notifications",
			[=, this](events::window_changed_event *ev) {
				widget->window_focused(ev->to);
			});

		pingpong::events::listen<events::window_notification_event>("p:notifications",
			[=, this](events::window_notification_event *ev) {
				if (ev->window == parent->get_ui().get_active_window())
					ev->window->highest_notification = notification_type::none;
				else
					parent->render_statusbar();
			});
	}

	void notifications_plugin::cleanup(plugin_host *) {
		config::unregister("appearance", "highlight_color");
		config::unregister("appearance", "highlight_bold");

		pingpong::events::unlisten<events::notification_event>("p:notifications");
		pingpong::events::unlisten<events::window_changed_event>("p:notifications");
		pingpong::events::unlisten<events::window_notification_event>("p:notifications");
		parent->remove_status_widget(widget);
	}
}

spjalla::plugins::notifications_plugin ext_plugin {};
