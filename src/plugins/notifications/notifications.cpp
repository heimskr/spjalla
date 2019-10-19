#include <unordered_map>

#include "spjalla/plugins/notifications.h"
#include "spjalla/plugins/notifications/widget.h"

#include "pingpong/core/util.h"
#include "pingpong/events/event.h"
#include "spjalla/core/client.h"
#include "spjalla/config/defaults.h"
#include "spjalla/events/notification.h"
#include "spjalla/events/window_changed.h"
#include "spjalla/plugins/plugin.h"

#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	void notifications_plugin::preinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }

		widget = std::make_shared<notifications_widget>(client, 20);

		config::register_key("appearance", "highlight_color", "red", config::validate_color,
			[&, client](config::database &, const config::value &value) {
				widget->highlight_color = ansi::get_color(value.string_());
				client->render_statusbar();
			}, "The text color for highlight notifications.");

		config::register_key("appearance", "highlight_bold", true, config::validate_bool,
			[&, client](config::database &, const config::value &value) {
				widget->highlight_bold = value.bool_();
				client->render_statusbar();
			}, "Whether to render highlight notifications in bold.");
	}

	void notifications_plugin::postinit(plugin_host *host) {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) { DBG("Error: expected client as plugin host"); return; }

		client->add_status_widget(widget);

		pingpong::events::listen<events::window_changed_event> ([=, this](events::window_changed_event *ev) {
			widget->window_focused(ev->to);
		});

		pingpong::events::listen<events::window_notification_event>([=](events::window_notification_event *ev) {
			if (ev->window == client->get_ui().get_active_window())
				ev->window->highest_notification = notification_type::none;
			else
				client->render_statusbar();
		});
	}
}

spjalla::plugins::notifications_plugin ext_plugin {};
