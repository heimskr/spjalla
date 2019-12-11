#include "pingpong/core/debug.h"

#include "spjalla/core/client.h"
#include "spjalla/core/plugin_host.h"

#include "spjalla/lines/raw.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"
#include "lib/haunted/lib/ustring.h"

namespace spjalla::plugins {
	struct extracmd_plugin: public plugin {
		~extracmd_plugin() {}

		std::string get_name()        const override { return "Extra Commands"; }
		std::string get_description() const override { return "Adds a few extra utility commands."; }
		std::string get_version()     const override { return "0.1.0"; }

		void postinit(plugin_host *host) override {
			parent = dynamic_cast<spjalla::client *>(host);
			if (!parent) { DBG("Error: expected client as plugin host"); return; }

			parent->add("rmraw", 0, 0, false, [this](pingpong::server *, const input_line &) {
				parent->get_ui().get_active_window()->remove_rows([](const haunted::ui::textline *line) -> bool {
					return dynamic_cast<const lines::raw_line *>(line);
				});
			});

			parent->add("_foo", 0, 0, false, [this](pingpong::server *serv, const input_line &) {
				ui::window *win = parent->get_ui().get_active_window();
				for (int i = 0; i < win->get_position().height - 2; ++i)
					*win += lines::raw_line(parent, "Foo " + std::to_string(i), serv);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				DBG("Go.");
				*win += lines::raw_line(parent, std::string(405, '.'), serv);
			});

			parent->add("_bar", 0, 0, false, [this](pingpong::server *serv, const input_line &) {
				*parent->get_ui().get_active_window() += lines::raw_line(parent, std::string(405, '.'), serv);
			});

			parent->add("_asc", 0, 0, false, [this](pingpong::server *, const input_line &) {
				ui::window *win = parent->get_ui().get_active_window();
				win->set_autoscroll(!win->get_autoscroll());
				DBG("Autoscroll is now " << ansi::bold(win->get_autoscroll()? "on" : "off") << ".");
			});

			parent->add("_args", 0, -1, false, [this](pingpong::server *, const input_line &il) {
				ui::interface &ui = parent->get_ui();
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
			});

			parent->add("_dbg", 0, 0, false, [this](pingpong::server *, const input_line &) {
				parent->debug_servers();
			});

			parent->add("_info", 0, 1, false, [this](pingpong::server *, const input_line &il) {
				if (il.args.size() == 0) {
					pingpong::debug::print_all(parent->get_irc());
					return;
				}

				const std::string &first = il.first();
				parent->get_ui().log("Unknown option: " + first);
			});

			parent->add("_sw", 0, 0, false, [this](pingpong::server *, const input_line &) {
				DBG("Number of status widgets: " << parent->get_status_widgets().size());
				for (const auto &widget: parent->get_status_widgets()) {
					DBG("- " << widget->get_name());
				}
			});

			parent->add("_time", 0, 0, false, [](pingpong::server *, const input_line &) {
				DBG("Default time: " << pingpong::util::timestamp());
				DBG("Seconds:      " << pingpong::util::seconds());
				DBG("Milliseconds: " << pingpong::util::millistamp());
				DBG("Microseconds: " << pingpong::util::microstamp());
				DBG("Nanoseconds:  " << pingpong::util::nanostamp());
			});

			parent->add("_users", 0, 0, true, [](pingpong::server *serv, const input_line &) {
				std::ostringstream to_print;
				to_print << serv->id << ":";
				for (std::shared_ptr<pingpong::user> user: serv->users) {
					to_print << " "  << user->name;
				}
				DBG(to_print.str());
			});

			parent->add("_win", 0, 0, false, [this](pingpong::server *, const input_line &) {
				if (ui::window *win = parent->get_ui().get_active_window()) {
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
			});

			parent->add("_findemoji", 0, 0, false, [this](pingpong::server *, const input_line &) {
				for (UChar32 i = 0; i < 99999; ++i) {
					haunted::ustring ustr = icu::UnicodeString::fromUTF32(&i, 1);
					DBG("[" << ustr << "] " << (u_hasBinaryProperty(i, UCHAR_EMOJI)? "true" : "false"));
				}
			});
		}

		void cleanup(plugin_host *) override {
			parent->remove_command("rmraw");
			parent->remove_command("_foo");
			parent->remove_command("_bar");
			parent->remove_command("_asc");
			parent->remove_command("_args");
			parent->remove_command("_dbg");
			parent->remove_command("_info");
			parent->remove_command("_sw");
			parent->remove_command("_time");
			parent->remove_command("_users");
			parent->remove_command("_win");
			parent->remove_command("_findemoji");
		}
	};
}

spjalla::plugins::extracmd_plugin ext_plugin {};
