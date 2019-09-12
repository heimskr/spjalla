#include <iostream>
#include <string>
#include <thread>

#include "formicine/ansi.h"

#include "haunted/core/util.h"

#include "pingpong/commands/all.h"
#include "pingpong/core/channel.h"
#include "pingpong/core/debug.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"
#include "pingpong/events/all.h"
#include "pingpong/messages/join.h"
#include "pingpong/messages/numeric.h"
#include "pingpong/messages/ping.h"

#include "core/client.h"
#include "core/input_line.h"

#include "lines/privmsg.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	client::~client() {
		term.join();
	}

	client & client::operator+=(const command_pair &p) {
		add(p);
		return *this;
	}
	
	client & client::operator+=(const server_ptr &ptr) {
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
						ui.log("Unknown command: /" + il.command);
					}
				} catch (std::exception &err) {
					ui.log("Caught an exception (" + haunted::util::demangle_object(err) + "): " + err.what());
				}
			} else {
				if (channel_ptr chan = ui.get_active_channel()) {
					privmsg_command(chan, str).send();
				} else {
					ui.log("No active channel.");
				}
			}
		});
	}

	bool client::handle_line(const input_line &il) {
		const int nargs = static_cast<int>(il.args.size());
		const std::string &name = il.command;

		auto range = command_handlers.equal_range(name);
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
				YIKES("No server is selected.");
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
		events::listen<message_event>([&](message_event *ev) {
			if (!ev->msg->is<numeric_message>() && !ev->msg->is<ping_message>())
				ui.log(*(ev->msg));
		});

		events::listen<raw_in_event>([&](raw_in_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap("<< ", ansi::color::gray) + ev->raw_in, 3));
		});

		events::listen<raw_out_event>([&](raw_out_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::lightgray) + ev->raw_out, 3));
		});

		events::listen<bad_line_event>([&](bad_line_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::red) + ev->bad_line, 3));
		});

		events::listen<join_event>([&](join_event *ev) {
			ui::window *win = ui.get_window(ev->chan, true);
			*win += "-!- "_d + ansi::bold(ev->who->name) + " joined " + ansi::bold(ev->chan->name);

			if (ev->who->name == ev->serv->get_nick())
				ui.focus_window(win);
		});

		events::listen<command_event>([&](command_event *ev) {
			if (privmsg_command *privmsg = dynamic_cast<privmsg_command *>(ev->cmd)) {
				*ui.get_window(privmsg->destination, true) += lines::privmsg_line(*privmsg);
			}
		});
	}

	void client::add_handlers() {
		using sptr = pingpong::server_ptr;
		using line = const input_line &;

		add<join_command>("join");

		add({"nick",  {0,  1, true, [&](sptr serv, line il) {
			if (il.args.size() == 0)
				ui.log("Current nick: " + serv->get_nick());
			else
				nick_command(serv, il.first()).send();
		}}});

		add({"msg",   {2, -1, true, [](sptr serv, line il) { msg_command(serv, il.first(), il.rest()).send(); }}});
		add({"quote", {1, -1, true, [](sptr serv, line il) { serv->quote(il.body);                            }}});

		add({"part",  {0, -1, true, [&](sptr serv, line il) {
			channel_ptr active_channel = ui.get_active_channel();

			if ((il.args.empty() || il.first()[0] != '#') && !active_channel) {
				ui.log("No active channel.");
			} else if (il.args.empty()) {
				part_command(serv, active_channel).send();
			} else if (il.first()[0] != '#') {
				part_command(serv, active_channel, il.body).send();
			} else if (channel_ptr cptr = serv->get_channel(il.first())) {
				part_command(serv, cptr, il.rest()).send();
			} else {
				ui.log(il.first() + ": no such channel.");
			}
		}}});

		add({"quit",  {0, -1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				for (auto serv: pp.servers)
					quit_command(serv).send();
			} else {
				for (auto serv: pp.servers)
					quit_command(serv, il.body).send();
			}

			stop();
		}}});

		add({"chans", {0, 0, true, [&](sptr serv, line) {
			std::string msg = "Channels:";
			for (auto [name, chan]: serv->channels)
				msg += " " + name;
			ui.log(msg);
		}}});

		add({"chan",  {0, 0, true, [&](sptr, line) {
			channel_ptr chan = ui.get_active_channel();
			if (chan == nullptr)
				ui.log("No active channel.");
			else
				ui.log("Active channel: " + chan->name);
		}}});

		add({"info",  {0, 1, false, [&](sptr, line il) {
			if (il.args.size() == 0) {
				debug::print_all(pp);
				return;
			}
			
			const std::string &first = il.first();
			ui.log("Unknown option: " + first);
		}}});

		add({"connect", {1, 2, false, [&](sptr, line il) {
			const std::string &hostname = il.first();

			std::string nick(irc::default_nick);
			if (il.args.size() > 1)
				nick = il.args[1];

			server *serv = new server(&pp, hostname);
			serv->start();
			serv->set_nick(nick);
			pp += serv;
		}}});

		add({"clear", {0, 0, false, [&](sptr, line) {
			if (ui::window *win = ui.get_active_window()) {
				// TODO: find out why changing the voffset has seemingly no effect.
				// win->set_voffset(win->total_rows());
				win->clear_lines();
				win->draw();
			} else {
				DBG("!! No window.");
			}
		}}});
	}

	server_ptr client::active_server() {
		return pp.active_server;
	}

	std::string client::active_nick() {
		if (server_ptr serv = active_server())
			return serv->get_nick();
		return std::string();
	}

	void client::stop() {
		alive = false;
	}
}
