#include <iostream>
#include <string>
#include <thread>

#include "haunted/core/util.h"

#include "pingpong/commands/join.h"
#include "pingpong/commands/nick.h"
#include "pingpong/commands/part.h"
#include "pingpong/commands/quit.h"

#include "pingpong/core/channel.h"
#include "pingpong/core/debug.h"
#include "pingpong/core/ppdefs.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"

#include "pingpong/events/bad_line.h"
#include "pingpong/events/command.h"
#include "pingpong/events/error.h"
#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/message.h"
#include "pingpong/events/names_updated.h"
#include "pingpong/events/nick.h"
#include "pingpong/events/part.h"
#include "pingpong/events/privmsg.h"
#include "pingpong/events/quit.h"
#include "pingpong/events/raw.h"
#include "pingpong/events/server_status.h"

#include "pingpong/messages/join.h"
#include "pingpong/messages/numeric.h"
#include "pingpong/messages/ping.h"

#include "pingpong/net/resolution_error.h"

#include "core/client.h"
#include "core/input_line.h"

#include "lines/join.h"
#include "lines/kick.h"
#include "lines/nick_change.h"
#include "lines/part.h"
#include "lines/privmsg.h"
#include "lines/quit.h"

#include "formicine/ansi.h"

long int strtol(const char *, char **, int base); // suppresses an incorrect VS Code error

namespace spjalla {
	client::~client() {
		term.join();
	}

	client & client::operator+=(const command_pair &p) {
		add(p);
		return *this;
	}
	
	client & client::operator+=(pingpong::server *ptr) {
		pp += ptr;
		return *this;
	}

	void client::init() {
		term.watch_size();
		ui.start();
		pp.init();
		add_listeners();
		add_handlers();
		term.start_input();

		ui.input->listen(haunted::ui::textinput::event::submit, [&](const haunted::superstring &sstr, int) {
			if (sstr.empty()) return;
			std::string str = sstr.str();
			ui.input->clear();
			input_line il = input_line(str);

			if (il.is_command()) {
				try {
					if (!handle_line(il)) {
						// If the command isn't an exact match, try partial matches (e.g., "/j" for "/join").
						const std::string &cmd = il.command;
						size_t cmd_length = cmd.length();

						std::vector<std::string> matches;

						for (std::pair<std::string, command_tuple> pair: command_handlers) {
							const std::string &candidate_name = pair.first;
							if (candidate_name.substr(0, cmd_length) == cmd)
								matches.push_back(candidate_name);
						}

						if (1 < matches.size()) {
							ui.log("Ambiguous command: /" + cmd);
							std::string joined;
							for (const std::string &match: matches)
								joined += "/" + match + " ";
							DBG("Matches: " << joined);
						} else {
							if (!matches.empty() && handle_line("/" + matches[0] + " " + il.body))
								return;
							ui.log("Unknown command: /" + cmd);
						}
					}
				} catch (std::exception &err) {
					ui.log(err);
				}
			} else {
				if (std::shared_ptr<pingpong::channel> chan = ui.get_active_channel()) {
					pingpong::privmsg_command(chan, str).send();
				} else if (std::shared_ptr<pingpong::user> user = ui.get_active_user()) {
					pingpong::privmsg_command(user, str).send();
				} else {
					ui.log(lines::red_notice + "No active channel.");
				}
			}
		});
	}

	bool client::handle_line(const input_line &il) {
		const int nargs = static_cast<int>(il.args.size());
		const std::string &name = il.command;

		auto range = command_handlers.equal_range(name);
		// Quit if there are no matching handlers.
		if (range.first == range.second)
			return false;

		for (auto it = range.first; it != range.second; ++it) {
			auto [min, max, needs_serv, fn] = it->second;
			if (max == 0 && nargs != 0) {
				YIKES("/" << name << " doesn't accept any arguments.");
			} else if (min == max && nargs != min) {
				YIKES("/" << name << " expects " << min << " argument" << (min == 1? "." : "s."));
			} else if (nargs < min) {
				YIKES("/" << name << " expects at least " << std::to_string(min) << " argument"
				      << (min == 1? "." : "s."));
			} else if (max != -1 && max < nargs) {
				YIKES("/" << name << " expects at most " << std::to_string(max) << " argument"
				      << (min == 1? "." : "s."));
			} else if (needs_serv && !pp.active_server) {
				ui.log(lines::red_notice + "No server is selected.");
			} else {
				fn(pp.active_server, il);
			}
		}

		return true;
	}

	void client::add(const command_pair &p) {
		command_handlers.insert(p);
	}

	void client::add_listeners() {
		pingpong::events::listen<pingpong::bad_line_event>([&](pingpong::bad_line_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::red) + ev->bad_line, 3));
		});

		pingpong::events::listen<pingpong::command_event>([&](pingpong::command_event *ev) {
			if (pingpong::quit_command *quit = dynamic_cast<pingpong::quit_command *>(ev->cmd))
				server_removed(quit->serv);
		});

		pingpong::events::listen<pingpong::error_event>([&](pingpong::error_event *ev) {
			ui::window *win = ev->current_window? ui.active_window : ui.status_window;
			*win += haunted::ui::simpleline(lines::red_notice + ev->message, ansi::length(lines::red_notice));
		});

		pingpong::events::listen<pingpong::join_event>([&](pingpong::join_event *ev) {
			ui::window *win = ui.get_window(ev->chan, true);
			*win += lines::join_line(*ev);

			if (ev->who->is_self())
				ui.focus_window(win);
		});

		pingpong::events::listen<pingpong::kick_event>([&](pingpong::kick_event *ev) {
			if (ui::window *win = ui.get_window(ev->chan, false)) {
				win->data->dead = true;
				*win += lines::kick_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::message_event>([&](pingpong::message_event *ev) {
			if (!ev->msg->is<pingpong::numeric_message>() && !ev->msg->is<pingpong::ping_message>())
				ui.log(*(ev->msg));
		});

		pingpong::events::listen<pingpong::names_updated_event>([&](pingpong::names_updated_event *ev) {
			if (ui::window *win = ui.get_active_window()) {
				if (win->data && win->data->chan && win->data->chan == ev->chan)
					ui.update_overlay();
			}
		});

		pingpong::events::listen<pingpong::nick_event>([&](pingpong::nick_event *ev) {
			lines::nick_change_line nline = lines::nick_change_line(*ev);
			for (ui::window *win: ui.windows_for_user(ev->who))
				*win += nline;
		});

		pingpong::events::listen<pingpong::part_event>([&](pingpong::part_event *ev) {
			if (ui::window *win = try_window(ev->chan)) {
				if (ev->who->is_self())
					ui.remove_window(win);
				else
					*win += lines::part_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::privmsg_event>([&](pingpong::privmsg_event *ev) {
			if (ev->is_channel()) {
				*ui.get_window(ev->get_channel(ev->serv), true) += lines::privmsg_line(*ev);
			} else {
				if (ev->speaker->is_self()) // privmsg_events are dispatched when we send messages too.
					*ui.get_window(ev->serv->get_user(ev->where, true), true) += lines::privmsg_line(*ev);
				else
					*ui.get_window(ev->speaker, true) += lines::privmsg_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::quit_event>([&](pingpong::quit_event *ev) {
			lines::quit_line qline = lines::quit_line(ev->who, ev->content, ev->stamp);
			for (ui::window *win: ui.windows_for_user(ev->who))
				*win += qline;
		});

		pingpong::events::listen<pingpong::raw_in_event>([&](pingpong::raw_in_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap("<< ", ansi::color::gray) + ev->raw_in, 3));
		});

		pingpong::events::listen<pingpong::raw_out_event>([&](pingpong::raw_out_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::lightgray) + ev->raw_out, 3));
		});

		pingpong::events::listen<pingpong::server_status_event>([&](pingpong::server_status_event *) {
			if (ui.active_window == ui.status_window)
				ui.update_overlay();
		});
	}

	void client::add_handlers() {
		using sptr = pingpong::server *;
		using line = const input_line &;

		add({"chans", {0, 0, true, [&](sptr serv, line) {
			std::string msg = "Channels:";
			for (std::shared_ptr<pingpong::channel> chan: serv->channels)
				msg += " " + chan->name;
			ui.log(msg);
		}}});

		add({"chan", {0, 0, true, [&](sptr, line) {
			std::shared_ptr<pingpong::channel> chan = ui.get_active_channel();
			if (chan == nullptr)
				ui.log(lines::red_notice + "No active channel.");
			else
				ui.log("Active channel: " + chan->name);
		}}});

		add({"clear", {0, 0, false, [&](sptr, line) {
			if (ui::window *win = ui.get_active_window()) {
				// TODO: find out why changing the voffset has seemingly no effect.
				win->set_voffset(win->total_rows());
				// win->clear_lines();
				// win->draw();
			} else {
				DBG(lines::red_notice + "No window.");
			}
		}}});

		add({"connect", {1, 2, false, [&](sptr, line il) {
			const std::string &hostname = il.first();

			std::string nick(pingpong::irc::default_nick);
			if (il.args.size() > 1)
				nick = il.args[1];

			pingpong::server *serv = new pingpong::server(&pp, hostname);
			try {
				serv->start();
				serv->set_nick(nick);
				pp += serv;
			} catch (pingpong::net::resolution_error &err) {
				delete serv;
				throw;
			}
		}}});

		add({"defnick", {0, 1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				ui.log("The current default nick is " + ansi::bold(pingpong::irc::default_nick));
			} else {
				pingpong::irc::default_nick = il.first();
				ui.log("Default nick set to " + ansi::bold(pingpong::irc::default_nick));
			}
		}}});

		add({"defuser", {0, 1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				ui.log("The current default user is " + ansi::bold(pingpong::irc::default_user));
			} else {
				pingpong::irc::default_user = il.first();
				ui.log("Default username set to " + ansi::bold(pingpong::irc::default_user));
			}
		}}});

		add({"defreal", {0, -1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				ui.log("The current default realname is " + ansi::bold(pingpong::irc::default_realname));
			} else {
				pingpong::irc::default_realname = il.body;
				ui.log("Default realname set to " + ansi::bold(pingpong::irc::default_realname));
			}
		}}});

		add({"info", {0, 1, false, [&](sptr, line il) {
			if (il.args.size() == 0) {
				pingpong::debug::print_all(pp);
				return;
			}
			
			const std::string &first = il.first();
			ui.log("Unknown option: " + first);
		}}});

		// add({"kick", {1, -1, true, [&](sptr, line il) {
			
		// }}});

		add({"me", {1, -1, true, [&](sptr, line il) {
			if (std::shared_ptr<pingpong::channel> chan = ui.get_active_channel())
				pingpong::privmsg_command(chan, "\1ACTION " + il.body + "\1").send();
			else ui.log(lines::red_notice + "No active channel.");
		}}});

		add({"msg", {2, -1, true, [&](sptr serv, line il) {
			pingpong::privmsg_command(serv, il.first(), il.rest()).send();
		}}});

		add<pingpong::join_command>("join");

		add({"nick", {0,  1, true, [&](sptr serv, line il) {
			if (il.args.size() == 0)
				ui.log("Current nick: " + serv->get_nick());
			else
				pingpong::nick_command(serv, il.first()).send();
		}}});

		add({"part", {0, -1, true, [&](sptr serv, line il) {
			std::shared_ptr<pingpong::channel> active_channel = ui.get_active_channel();

			if ((il.args.empty() || il.first()[0] != '#') && !active_channel) {
				ui.log(lines::red_notice + "No active channel.");
			} else if (il.args.empty()) {
				pingpong::part_command(serv, active_channel).send();
			} else if (il.first()[0] != '#') {
				pingpong::part_command(serv, active_channel, il.body).send();
			} else if (std::shared_ptr<pingpong::channel> cptr = serv->get_channel(il.first())) {
				pingpong::part_command(serv, cptr, il.rest()).send();
			} else {
				ui.log(il.first() + ": no such channel.");
			}
		}}});

		add({"quit", {0, -1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				for (auto serv: pp.servers)
					pingpong::quit_command(serv).send();
			} else {
				for (auto serv: pp.servers)
					pingpong::quit_command(serv, il.body).send();
			}

			stop();
		}}});

		add({"quote", {1, -1, true, [&](sptr serv, line il) {
			serv->quote(il.body);
		}}});

		add({"spam", {0, 1, false, [&](sptr, line il) {
			long max = 64;

			if (!il.args.empty()) {
				char *ptr;
				const std::string &str = il.first();
				long parsed = strtol(str.c_str(), &ptr, 10);
				if (ptr != 1 + &*(str.end() - 1)) {
					ui.log(ansi::yellow("!!") + " Invalid number: \"" + il.first() + "\"");
				} else max = parsed;
			}

			for (long i = 1; i <= max; ++i)
				ui.log(std::to_string(i));
		}}});

		add({"swap", {0, 0, false, [&](sptr, line) {
			ui.set_overlay_side(ui.overlay_side == haunted::side::left? haunted::side::right : haunted::side::left);
		}}});
		
		add({"dbg", {0, 0, false, [&](sptr, line) {
			debug_servers();
		}}});

		add({"overlay", {0, 0, false, [&](sptr, line) {
			ui.update_overlay();
		}}});
	}

	void client::server_removed(pingpong::server *serv) {
		// We need to check the windows in reverse because we're removing some along the way. Removing elements while
		// looping through a vector causes all kinds of problems unless you loop in reverse.
		std::vector<haunted::ui::control *> &windows = ui.swappo->get_children();
		for (auto iter = windows.rbegin(), rend = windows.rend(); iter != rend; ++iter) {
			ui::window *win = dynamic_cast<ui::window *>(*iter);
			if (win->data && win->data->serv == serv) {
				ui.remove_window(win);
			}
		}
	}

	void client::join() {
		term.join();
	}

	pingpong::server * client::active_server() {
		return pp.active_server;
	}

	std::string client::active_nick() {
		if (pingpong::server *serv = active_server())
			return serv->get_nick();
		return std::string();
	}

	void client::stop() {
		alive = false;
	}

	void client::debug_servers() {
		for (pingpong::server *serv: pp.servers) {
			DBG(ansi::bold(serv->hostname));
			for (std::shared_ptr<pingpong::channel> chan: serv->channels) {
				DBG("    " << ansi::wrap(chan->name, ansi::style::underline));
				for (std::shared_ptr<pingpong::user> user: chan->users) {
					std::string chans = "";
					for (std::weak_ptr<pingpong::channel> user_chan: user->channels)
						chans += " " + user_chan.lock()->name;
					if (chans.empty())
						DBG("        " << user->name);
					else
						DBG("        " << user->name << ":" << chans);
				}
			}
		}
	}

	ui::window * client::try_window(std::shared_ptr<pingpong::channel> chan) {
		ui::window *win = ui.get_window(chan, false);
		if (!win)
			ui.log(ansi::yellow("!!") + " Couldn't find window for " + std::string(*chan));
		return win;
	}
}
