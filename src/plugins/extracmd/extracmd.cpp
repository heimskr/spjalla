#include "pingpong/core/Debug.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/PluginHost.h"

#include "spjalla/lines/Raw.h"

#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/ansi.h"
#include "lib/haunted/lib/ustring.h"

namespace Spjalla::Plugins {
	struct ExtraCmdPlugin: public Plugin {
		~ExtraCmdPlugin() {}

		std::string getName()        const override { return "Extra Commands"; }
		std::string getDescription() const override { return "Adds a few extra utility commands."; }
		std::string getVersion()     const override { return "0.1.0"; }

		void postinit(PluginHost *host) override {
			parent = dynamic_cast<Spjalla::Client *>(host);
			if (!parent) { DBG("Error: expected client as plugin host"); return; }

			parent->add("rmraw", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				parent->getUI().getActiveWindow()->removeRows([](const Haunted::UI::TextLine *line) -> bool {
					return dynamic_cast<const Lines::RawLine *>(line);
				});
			});

			parent->add("_foo", 0, 0, false, [this](PingPong::Server *server, const InputLine &) {
				UI::Window *window = parent->getUI().getActiveWindow();
				for (int i = 0; i < window->getPosition().height - 2; ++i)
					*window += Lines::RawLine(parent, "Foo " + std::to_string(i), server);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				DBG("Go.");
				*window += Lines::RawLine(parent, std::string(405, '.'), server);
			});

			parent->add("_bar", 0, 0, false, [this](PingPong::Server *server, const InputLine &) {
				*parent->getUI().getActiveWindow() += Lines::RawLine(parent, std::string(405, '.'), server);
			});

			parent->add("_asc", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				UI::Window *window = parent->getUI().getActiveWindow();
				window->setAutoscroll(!window->getAutoscroll());
				DBG("Autoscroll is now " << ansi::bold(window->getAutoscroll()? "on" : "off") << ".");
			});

			parent->add("_args", 0, -1, false, [this](PingPong::Server *, const InputLine &il) {
				UI::Interface &ui = parent->getUI();
				std::vector<std::string> strings {};
				ui.log("Body: " + "\""_d + Util::escape(il.body) + "\""_d);
				DBG("Body: " + "\""_d + Util::escape(il.body) + "\""_d);
				if (il.args.empty()) {
					ui.log("Args: " + "(none)"_d, ui.getActiveWindow());
					DBG("Args: " + "(none)"_d);
					return;
				}

				std::transform(il.args.begin(), il.args.end(), std::back_inserter(strings), [](const std::string &str) {
					return "\""_d + Util::escape(str) + "\""_d;
				});
				std::string joined = "Args: " + formicine::util::join(strings.begin(), strings.end());
				ui.log(joined, ui.getActiveWindow());
				DBG(joined);
			});

			parent->add("_dbg", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				parent->debugServers();
			});

			parent->add("_info", 0, 1, false, [this](PingPong::Server *, const InputLine &il) {
				if (il.args.size() == 0) {
					PingPong::Debug::printAll(parent->getIRC());
					return;
				}

				const std::string &first = il.first();
				parent->getUI().log("Unknown option: " + first);
			});

			parent->add("_sw", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				DBG("Number of status widgets: " << parent->getStatusWidgets().size());
				for (const auto &widget: parent->getStatusWidgets()) {
					DBG("- " << widget->getName());
				}
			});

			parent->add("_time", 0, 0, false, [](PingPong::Server *, const InputLine &) {
				DBG("Default time: " << PingPong::Util::timestamp());
				DBG("Seconds:      " << PingPong::Util::seconds());
				DBG("Milliseconds: " << PingPong::Util::millistamp());
				DBG("Microseconds: " << PingPong::Util::microstamp());
				DBG("Nanoseconds:  " << PingPong::Util::nanostamp());
			});

			parent->add("_users", 0, 0, true, [](PingPong::Server *server, const InputLine &) {
				std::ostringstream to_print;
				to_print << server->id << ":";
				for (std::shared_ptr<PingPong::User> user: server->users) {
					to_print << " "  << user->name;
				}
				DBG(to_print.str());
			});

			parent->add("_win", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
				if (UI::Window *win = parent->getUI().getActiveWindow()) {
					DBG("Window name:    " << ansi::bold(win->windowName));
					switch (win->type) {
						case UI::WindowType::Channel: DBG("Window type:    " << "channel"_b); break;
						case UI::WindowType::User:    DBG("Window type:    " << "user"_b);    break;
						case UI::WindowType::Status:  DBG("Window type:    " << "status"_b);  break;
						case UI::WindowType::Overlay: DBG("Window type:    " << "overlay"_b); break;
						case UI::WindowType::Other:   DBG("Window type:    " << "other"_b);   break;
						default: DBG("Window type:    " << "invalid"_d);
					}
					DBG("Window user:    " << (win->user?    win->user->name    : "null"_d));
					DBG("Window channel: " << (win->channel? win->channel->name : "null"_d));
					DBG("Window server:  " << (win->server?  win->server->id    : "null"_d));
					switch (win->highestNotification) {
						case NotificationType::None:      DBG("Window highest: " << "none"_b);      break;
						case NotificationType::Info:      DBG("Window highest: " << "info"_b);      break;
						case NotificationType::Message:   DBG("Window highest: " << "message"_b);   break;
						case NotificationType::Highlight: DBG("Window highest: " << "highlight"_b); break;
						default: DBG("Window highest: " << "invalid"_d);
					}
					DBG("Window voffset: " << win->getVoffset());
					DBG("Window autoscroll: " << (win->getAutoscroll()? "true" : "false"));
				} else {
					DBG("Window: " << "null"_d);
				}
			});

			parent->add("_findemoji", 0, 0, false, [](PingPong::Server *, const InputLine &) {
#ifdef ENABLE_ICU
				for (UChar32 i = 0; i < 99999; ++i) {
					Haunted::ustring ustr = icu::UnicodeString::fromUTF32(&i, 1);
					DBG("[" << ustr << "] " << (u_hasBinaryProperty(i, UCHAR_EMOJI)? "true" : "false"));
				}
#else
				DBG("ICU support not enabled.");
#endif
			});
		}

		void cleanup(PluginHost *) override {
			parent->removeCommand("rmraw");
			parent->removeCommand("_foo");
			parent->removeCommand("_bar");
			parent->removeCommand("_asc");
			parent->removeCommand("_args");
			parent->removeCommand("_dbg");
			parent->removeCommand("_info");
			parent->removeCommand("_sw");
			parent->removeCommand("_time");
			parent->removeCommand("_users");
			parent->removeCommand("_win");
			parent->removeCommand("_findemoji");
		}
	};
}

Spjalla::Plugins::ExtraCmdPlugin ext_plugin {};
