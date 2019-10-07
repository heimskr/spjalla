#include <unordered_map>

#include "pingpong/core/util.h"
#include "pingpong/events/event.h"

#include "spjalla/config/defaults.h"
#include "spjalla/core/client.h"
#include "spjalla/events/notification.h"
#include "spjalla/events/window_changed.h"
#include "spjalla/plugins/plugin.h"
#include "spjalla/ui/status_widget.h"

#include "lib/formicine/ansi.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins {
	class notifications_widget: public spjalla::ui::status_widget {
		public:
			ansi::color highlight_color = ansi::color::yellow;
			bool highlight_bold = true;

			using status_widget::status_widget;

			virtual ~notifications_widget() {}

			std::string render(const ui::window *, bool) const {
				std::vector<std::string> indicators;

				std::deque<haunted::ui::control *> controls = parent->get_window_controls();
				std::sort(controls.begin(), controls.end(), [](haunted::ui::control *one, haunted::ui::control *two) {
					return one->get_index() < two->get_index();
				});

				for (haunted::ui::control *control: controls) {
					const ssize_t index = control->get_index();
					if (index < 0)
						continue;

					ui::window *window = dynamic_cast<ui::window *>(control);
					if (window == parent->get_ui().get_active_window())
						continue;

					notification_type type = window->highest_notification;
					if (type == notification_type::none) {
						continue;
					}
					
					std::string index_str = std::to_string(index + 1);
					switch (type) {
						case notification_type::info:      indicators.push_back(ansi::dim(index_str)); break;
						case notification_type::message:   indicators.push_back(index_str); break;
						case notification_type::highlight: 
							indicators.push_back(ansi::wrap(highlight_bold? ansi::bold(index_str) : index_str,
								highlight_color));
							break;
						default: throw std::invalid_argument("Invalid notification type");
					}
				}

				return formicine::util::join(indicators.begin(), indicators.end(), ","_d);
			}

			void window_focused(ui::window *window) {
				window->unnotify();
			}
	};

	class notifications_widget_plugin: public plugin {
		private:
			std::shared_ptr<notifications_widget> widget;

		public:
			virtual ~notifications_widget_plugin() {}

			std::string get_name()        const override { return "Notifications"; }
			std::string get_description() const override { return "Shows a notifications widget in the status bar."; }
			std::string get_version()     const override { return "0.1.1"; }

			void preinit(plugin_host *host) override {
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

			void postinit(plugin_host *host) override {
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
	};
}

spjalla::plugins::notifications_widget_plugin ext_plugin {};
