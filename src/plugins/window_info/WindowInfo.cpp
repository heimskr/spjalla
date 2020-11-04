#include "pingpong/core/Util.h"
#include "pingpong/events/nick_updated.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"
#include "spjalla/plugins/Plugin.h"
#include "spjalla/ui/StatusWidget.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	class window_info_left_widget: public spjalla::ui::status_widget {
		private:
			long stamp = PingPong::Util::timestamp();

		protected:
			std::string _render(const UI::Window *win, bool) const override {
				if (!win)
					return "null?";

				std::string index = ansi::dim(ansi::bold(std::to_string(win->get_index() + 1))) + " ";

				if (win->is_status()) {
					return index + "status";
				} else if (win->is_channel()) {
					return index + util::colorize_if_dead(win->chan->name, win);
				} else if (win->is_user()) {
					return index + util::colorize_if_dead(win->user->name, win);
				} else {
					return ansi::bold(win->window_name);
				}
			}

		public:
			using status_widget::status_widget;

			virtual ~window_info_left_widget() {}

			const char * get_name() const override { return "Window Info (left)"; }
	};

	class window_info_right_widget: public spjalla::ui::status_widget {
		private:
			long stamp = PingPong::Util::timestamp();

		protected:
			std::string _render(const UI::Window *win, bool) const override {
				if (!win) {
					return "null?";
				} else if (win->is_status() || win->is_channel() || win->is_user()) {
					const std::string id = win->serv? win->server->id : parent->active_server_id();
					return id.empty()? "none"_i : id;
					return parent->active_server_id();
				} else {
					return "~";
				}
			}

		public:
			using status_widget::status_widget;

			virtual ~window_info_right_widget() {}

			const char * get_name() const override { return "Window Info (right)"; }
	};

	class window_info_widget_plugin: public plugin {
		private:
			std::shared_ptr<window_info_left_widget> widget_left;
			std::shared_ptr<window_info_right_widget> widget_right;

		public:
			virtual ~window_info_widget_plugin() {}

			std::string get_name()        const override { return "Window Name"; }
			std::string get_description() const override { return "Shows the name of the current window in the status"
				" bar."; }
			std::string get_version()     const override { return "0.1.0"; }

			void postinit(PluginHost *host) override {
				parent = dynamic_cast<Spjalla::Client *>(host);
				if (!parent) { DBG("Error: expected client as plugin host"); return; }

				widget_left  = std::make_shared<window_info_left_widget> (parent, 10);
				widget_right = std::make_shared<window_info_right_widget>(parent, 15);
				parent->add_status_widget(widget_left);
				parent->add_status_widget(widget_right);

				PingPong::Events::listen<PingPong::nick_updated_event>("p:window_info",
					[&, this](PingPong::nick_updated_event *ev) {
						// If this user's window is open, redraw the statusbar to update the widget.
						if (parent->getUI().get_active_user() == ev->who)
							parent->getUI().update_statusbar();
					});
			}

			void cleanup(PluginHost *) override {
				PingPong::Events::unlisten<PingPong::nick_updated_event>("p:window_info");
				parent->remove_status_widget(widget_left);
				parent->remove_status_widget(widget_right);
			}
	};
}

spjalla::plugins::window_info_widget_plugin ext_plugin {};
