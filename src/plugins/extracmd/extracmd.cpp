#include "pingpong/core/Debug.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/PluginHost.h"

#include "spjalla/lines/raw.h"

#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/ansi.h"
#include "lib/haunted/lib/ustring.h"

namespace Spjalla::Plugins {
	struct extracmd_plugin: public plugin {
		~extracmd_plugin() {}

		std::string get_name()        const override { return "Extra Commands"; }
		std::string get_description() const override { return "Adds a few extra utility commands."; }
		std::string get_version()     const override { return "0.1.0"; }

		void postinit(PluginHost *host) override {
			parent = dynamic_cast<Spjalla::Client *>(host);
			if (!parent) { DBG("Error: expected client as plugin host"); return; }

			parent->add("rmraw", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				parent->getUI().get_active_window()->remove_rows([](const Haunted::UI::textline *line) -> bool {
					return dynamic_cast<const lines::raw_line *>(line);
				});
			});

			parent->add("_foo", 0, 0, false, [this](PingPong::Server *serv, const InputLine &) {
				UI::Window *win = parent->getUI().get_active_window();
				for (int i = 0; i < win->get_position().height - 2; ++i)
					*win += lines::raw_line(parent, "Foo " + std::to_string(i), serv);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				DBG("Go.");
				*win += lines::raw_line(parent, std::string(405, '.'), serv);
			});

			parent->add("_bar", 0, 0, false, [this](PingPong::Server *serv, const InputLine &) {
				*parent->getUI().get_active_window() += lines::raw_line(parent, std::string(405, '.'), serv);
			});

			parent->add("_asc", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				UI::Window *win = parent->getUI().get_active_window();
				win->set_autoscroll(!win->get_autoscroll());
				DBG("Autoscroll is now " << ansi::bold(win->get_autoscroll()? "on" : "off") << ".");
			});

			parent->add("_args", 0, -1, false, [this](PingPong::Server *, const InputLine &il) {
				UI::Interface &ui = parent->getUI();
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

			parent->add("_dbg", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				parent->debug_servers();
			});

			parent->add("_info", 0, 1, false, [this](PingPong::Server *, const InputLine &il) {
				if (il.args.size() == 0) {
					PingPong::debug::print_all(parent->getIRC());
					return;
				}

				const std::string &first = il.first();
				parent->getUI().log("Unknown option: " + first);
			});

			parent->add("_sw", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				DBG("Number of status widgets: " << parent->getStatusWidgets().size());
				for (const auto &widget: parent->getStatusWidgets()) {
					DBG("- " << widget->get_name());
				}
			});

			parent->add("_time", 0, 0, false, [](PingPong::Server *, const InputLine &) {
				DBG("Default time: " << PingPong::Util::timestamp());
				DBG("Seconds:      " << PingPong::Util::seconds());
				DBG("Milliseconds: " << PingPong::Util::millistamp());
				DBG("Microseconds: " << PingPong::Util::microstamp());
				DBG("Nanoseconds:  " << PingPong::Util::nanostamp());
			});

			parent->add("_users", 0, 0, true, [](PingPong::Server *serv, const InputLine &) {
				std::ostringstream to_print;
				to_print << server->id << ":";
				for (std::shared_ptr<PingPong::User> user: server->users) {
					to_print << " "  << user->name;
				}
				DBG(to_print.str());
			});

			parent->add("_win", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				if (UI::Window *win = parent->getUI().get_active_window()) {
					DBG("Window name:    " << ansi::bold(win->window_name));
					switch (win->type) {
						case UI::WindowType::channel: DBG("Window type:    " << "channel"_b); break;
						case UI::WindowType::user:    DBG("Window type:    " << "user"_b);    break;
						case UI::WindowType::status:  DBG("Window type:    " << "status"_b);  break;
						case UI::WindowType::overlay: DBG("Window type:    " << "overlay"_b); break;
						case UI::WindowType::other:   DBG("Window type:    " << "other"_b);   break;
						default: DBG("Window type:    " << "invalid"_d);
					}
					DBG("Window user:    " << (win->user? win->user->name : "null"_d));
					DBG("Window channel: " << (win->chan? win->chan->name : "null"_d));
					DBG("Window server:  " << (win->serv? win->server->id : "null"_d));
					switch (win->highest_notification) {
						case NotificationType::none:      DBG("Window highest: " << "none"_b);      break;
						case NotificationType::Info:      DBG("Window highest: " << "info"_b);      break;
						case NotificationType::message:   DBG("Window highest: " << "message"_b);   break;
						case NotificationType::highlight: DBG("Window highest: " << "highlight"_b); break;
						default: DBG("Window highest: " << "invalid"_d);
					}
					DBG("Window voffset: " << win->get_voffset());
					DBG("Window autoscroll: " << (win->get_autoscroll()? "true" : "false"));
				} else {
					DBG("Window: " << "null"_d);
				}
			});

			parent->add("_findemoji", 0, 0, false, [](PingPong::Server *, const InputLine &) {
				for (UChar32 i = 0; i < 99999; ++i) {
					Haunted::ustring ustr = icu::UnicodeString::fromUTF32(&i, 1);
					DBG("[" << ustr << "] " << (u_hasBinaryProperty(i, UCHAR_EMOJI)? "true" : "false"));
				}
			});
		}

		void cleanup(PluginHost *) override {
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
