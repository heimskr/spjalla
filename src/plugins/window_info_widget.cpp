#include "pingpong/core/pputil.h"

#include "core/client.h"
#include "core/sputil.h"
#include "plugins/plugin.h"
#include "ui/status_widget.h"

#include "formicine/ansi.h"

namespace spjalla::plugins {
	class window_info_widget: public spjalla::ui::status_widget {
		private:
			long stamp = pingpong::util::timestamp();

		public:
			using status_widget::status_widget;

			virtual ~window_info_widget() {}

			std::string render(const ui::window *win, bool) const {
				if (!win) {
					return "null?";
				} else if (win->is_status()) {
					const std::string id = parent->active_server_id();
					return win->window_name + "] ["_d + (id.empty()? "none"_i : id);
				} else if (win->is_channel()) {
					return parent->active_server_id() + "] ["_d + util::colorize_if_dead(win->data.chan->name, win);
				} else if (win->is_user()) {
					return parent->active_server_id() + "] ["_d + util::colorize_if_dead(win->data.user->name, win);
				} else {
					return ansi::bold(win->window_name);
				}
			}
	};

	class window_info_widget_plugin: public plugin {
		private:
			std::shared_ptr<window_info_widget> widget;

		public:
			virtual ~window_info_widget_plugin() {}

			std::string get_name()        const override { return "Window name"; }
			std::string get_description() const override { return "Shows the name of the current window in the status"
				" bar."; }
			std::string get_version()     const override { return "0.0.0"; }

			void startup(plugin_host *host) override {
				spjalla::client *client = dynamic_cast<spjalla::client *>(host);
				if (!client) {
					DBG("Error: expected client as plugin host");
					return;
				}

				widget = std::make_shared<window_info_widget>(client, 10);
				client->add_status_widget(widget);
			}
	};
}

spjalla::plugins::window_info_widget_plugin ext_plugin {};
