#include "pingpong/core/debug.h"

#include "spjalla/core/client.h"
#include "spjalla/core/plugin_host.h"

#include "spjalla/lines/raw.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	struct extracmd_plugin: public plugin {
		~extracmd_plugin() {}

		std::string get_name()        const override { return "Extra Commands"; }
		std::string get_description() const override { return "Adds a few extra utility commands."; }
		std::string get_version()     const override { return "0.0.1"; }

		void postinit(plugin_host *host) override {
			spjalla::client *client = dynamic_cast<spjalla::client *>(host);
			if (!client) { DBG("Error: expected client as plugin host"); return; }

			client->add({"rmraw", {0, 0, false, [client](pingpong::server *, const input_line &) {
				client->get_ui().get_active_window()->remove_rows([](const haunted::ui::textline *line) -> bool {
					return dynamic_cast<const lines::raw_line *>(line);
				});
			}, {}}});

			client->add({"_foo", {0, 0, false, [client](pingpong::server *, const input_line &) {
				ui::window *win = client->get_ui().get_active_window();
				for (int i = 0; i < win->get_position().height - 2; ++i)
					*win += lines::raw_line(client, "Foo " + std::to_string(i));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				DBG("Go.");
				*win += lines::raw_line(client, std::string(405, '.'));
			}, {}}});

			client->add({"_bar", {0, 0, false, [client](pingpong::server *, const input_line &) {
				*client->get_ui().get_active_window() += lines::raw_line(client, std::string(405, '.'));
			}, {}}});

			client->add({"_asc", {0, 0, false, [client](pingpong::server *, const input_line &) {
				ui::window *win = client->get_ui().get_active_window();
				win->set_autoscroll(!win->get_autoscroll());
				DBG("Autoscroll is now " << ansi::bold(win->get_autoscroll()? "on" : "off") << ".");
			}, {}}});

			client->add({"_args", {0, -1, false, [client](pingpong::server *, const input_line &il) {
				ui::interface &ui = client->get_ui();
				std::vector<std::string> strings {};
				ui.log("Body: " + "\""_d + util::escape(il.body) + "\""_d);
				DBG("Body: " + "\""_d + util::escape(il.body) + "\""_d);
				if (il.args.empty()) {
					ui.log("Args: " + "(none)"_d, ui.get_active_window());
					DBG("Args: " + "(none)"_d);
					return;
				}

				std::transform(il.args.begin(), il.args.end(), std::back_inserter(strings), [](const std::string &str) {
					return "\""_d + util::escape(str) + "\""_d;
				});
				std::string joined = "Args: " + formicine::util::join(strings.begin(), strings.end());
				ui.log(joined, ui.get_active_window());
				DBG(joined);
			}, {}}});

			client->add({"_dbg", {0, 0, false, [client](pingpong::server *, const input_line &) {
				client->debug_servers();
			}, {}}});

			client->add({"_info", {0, 1, false, [client](pingpong::server *, const input_line &il) {
				if (il.args.size() == 0) {
					pingpong::debug::print_all(client->get_irc());
					return;
				}

				const std::string &first = il.first();
				client->get_ui().log("Unknown option: " + first);
			}, {}}});

			client->add({"_time", {0, 0, false, [](pingpong::server *, const input_line &) {
				DBG("Default time: " << pingpong::util::timestamp());
				DBG("Seconds:      " << pingpong::util::seconds());
				DBG("Milliseconds: " << pingpong::util::millistamp());
				DBG("Microseconds: " << pingpong::util::microstamp());
				DBG("Nanoseconds:  " << pingpong::util::nanostamp());
			}, {}}});

			client->add({"_win", {0, 0, false, [client](pingpong::server *, const input_line &) {
				if (ui::window *win = client->get_ui().get_active_window()) {
					DBG("Window name:    " << ansi::bold(win->window_name));
					switch (win->type) {
						case ui::window_type::channel: DBG("Window type:    " << "channel"_b); break;
						case ui::window_type::user:    DBG("Window type:    " << "user"_b);    break;
						case ui::window_type::status:  DBG("Window type:    " << "status"_b);  break;
						case ui::window_type::overlay: DBG("Window type:    " << "overlay"_b); break;
						case ui::window_type::other:   DBG("Window type:    " << "other"_b);   break;
						default: DBG("Window type:    " << "invalid"_d);
					}
					DBG("Window user:    " << (win->user? win->user->name : "null"_d));
					DBG("Window channel: " << (win->chan? win->chan->name : "null"_d));
					DBG("Window server:  " << (win->serv? win->serv->id : "null"_d));
					switch (win->highest_notification) {
						case notification_type::none:      DBG("Window highest: " << "none"_b);      break;
						case notification_type::info:      DBG("Window highest: " << "info"_b);      break;
						case notification_type::message:   DBG("Window highest: " << "message"_b);   break;
						case notification_type::highlight: DBG("Window highest: " << "highlight"_b); break;
						default: DBG("Window highest: " << "invalid"_d);
					}
					DBG("Window voffset: " << win->get_voffset());
					DBG("Window autoscroll: " << (win->get_autoscroll()? "true" : "false"));
				} else {
					DBG("Window: " << "null"_d);
				}
			}, {}}});
		}
	};
}

spjalla::plugins::extracmd_plugin ext_plugin {};
