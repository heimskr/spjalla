#include "pingpong/core/util.h"

#include "spjalla/core/client.h"
#include "spjalla/core/util.h"
#include "spjalla/plugins/plugin.h"
#include "spjalla/ui/status_widget.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	class window_info_left_widget: public spjalla::ui::status_widget {
		private:
			long stamp = pingpong::util::timestamp();

		public:
			using status_widget::status_widget;

			virtual ~window_info_left_widget() {}

			std::string render(const ui::window *win, bool) const {
				if (!win)
					return "null?";

				std::string index = ansi::dim(ansi::bold(std::to_string(win->get_index() + 1))) + " ";

				if (win->is_status()) {
					return index + "status";
				} else if (win->is_channel()) {
					return index + util::colorize_if_dead(win->data.chan->name, win);
				} else if (win->is_user()) {
					return index + util::colorize_if_dead(win->data.user->name, win);
				} else {
					return ansi::bold(win->window_name);
				}
			}
	};

	class window_info_right_widget: public spjalla::ui::status_widget {
		private:
			long stamp = pingpong::util::timestamp();

		public:
			using status_widget::status_widget;

			virtual ~window_info_right_widget() {}

			std::string render(const ui::window *win, bool) const {
				if (!win) {
					return "null?";
				} else if (win->is_status() || win->is_channel() || win->is_user()) {
					const std::string id = parent->active_server_id();
					return id.empty()? "none"_i : id;
					return parent->active_server_id();
				} else {
					return "~";
				}
			}
	};

	class window_info_widget_plugin: public plugin {
		private:
			std::shared_ptr<window_info_left_widget> widget_left;
			std::shared_ptr<window_info_right_widget> widget_right;

		public:
			virtual ~window_info_widget_plugin() {}

			std::string get_name()        const override { return "Window name"; }
			std::string get_description() const override { return "Shows the name of the current window in the status"
				" bar."; }
			std::string get_version()     const override { return "0.0.0"; }

			void postinit(plugin_host *host) override {
				spjalla::client *client = dynamic_cast<spjalla::client *>(host);
				if (!client) {
					DBG("Error: expected client as plugin host");
					return;
				}

				widget_left  = std::make_shared<window_info_left_widget> (client, 10);
				widget_right = std::make_shared<window_info_right_widget>(client, 15);
				client->add_status_widget(widget_left);
				client->add_status_widget(widget_right);
			}
	};
}

spjalla::plugins::window_info_widget_plugin ext_plugin {};
