#include <iostream>
#include <string>
#include <thread>

#include "commands/all.h"
#include "core/channel.h"
#include "core/debug.h"
#include "core/defs.h"
#include "core/irc.h"
#include "core/server.h"
#include "events/event.h"
#include "events/message.h"
#include "lib/ansi.h"
#include "messages/numeric.h"
#include "messages/ping.h"

#include "core/client.h"
#include "core/input_line.h"

using namespace pingpong;
using namespace spjalla;

namespace spjalla {
	client::~client() {
		DBG(ansi::wrap("client::~client()", ansi::color::red) << ": joining.");
		ui.join();
		DBG(ansi::wrap("client::~client()", ansi::color::red) << ": joined ui.");
		term.join();
		DBG(ansi::wrap("client::~client()", ansi::color::red) << ": joined term.");
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
	}

	// void client::input_worker() {
		// std::string in;

		// while (alive && std::getline(std::cin, in)) {
		// 	input_line il = input_line(in);

		// 	if (il.is_command()) {
		// 		try {
		// 			if (!handle_line(il)) {
		// 				std::cerr << "Unknown command: /" << il.command << "\r\n";
		// 			}
		// 		} catch (std::exception &err) {
		// 			YIKES("Caught an exception (" << typeid(err).name() << "): " << err.what());
		// 		}
		// 	} else if (channel_ptr chan = active_channel()) {
		// 		privmsg_command(*chan, in).send(true);
		// 		pp.dbgout() << "[" << *chan << "] <" << active_nick() << "> " << in << "\r\n";
		// 	} else {
		// 		YIKES("No active channel.");
		// 	}
		// }
	// }

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
		events::listen<message_event>([&](auto *ev) {
			if (!ev->msg->template is<numeric_message>() && !ev->msg->template is<ping_message>())
				pp.dbgout() << std::string(*(ev->msg)) << "\r\n";
		});
	}

	void client::add_handlers() {
		using sptr = pingpong::server_ptr;
		using line = const input_line &;

		add<join_command>("join");

		add({"nick",  {0,  1, true, [](sptr serv, line il) {
			if (il.args.size() == 0)
				std::cout << "Current nick: " << serv->get_nick() << "\r\n";
			else
				nick_command(serv, il.first()).send(true);
		}}});
		add({"msg",   {2, -1, true, [](sptr serv, line il) { msg_command(serv, il.first(), il.rest()).send(true); }}});
		add({"quote", {1, -1, true, [](sptr serv, line il) { serv->quote(il.body);                                }}});
		add({"part",  {0, -1, true, [](sptr serv, line il) {
			if ((il.args.empty() || il.first()[0] != '#') && !serv->active_channel) {
				std::cout << "No active channel.\r\n";
			} else if (il.args.empty()) {
				part_command(serv, serv->active_channel).send(true);
			} else if (il.first()[0] != '#') {
				part_command(serv, serv->active_channel, il.body).send(true);
			} else if (channel_ptr cptr = serv->get_channel(il.first())) {
				part_command(serv, cptr, il.rest()).send(true);
			} else {
				YIKES(il.first() << ": no such channel.");
			}
		}}});
		add({"quit",  {0, -1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				for (auto serv: pp.servers)
					quit_command(serv).send(true);
			} else {
				for (auto serv: pp.servers)
					quit_command(serv, il.body).send(true);
			}

			stop();
		}}});
		add({"chans", {0, 0, true, [](sptr serv, line) {
			std::cout << "Channels:";
			for (auto [name, chan]: serv->channels)
				std::cout << " " << name;
			std::cout << "\r\n";
		}}});
		add({"chan",  {0, 0, true, [](sptr serv, line) {
			channel_ptr chan = serv->active_channel;
			if (chan == nullptr)
				std::cout << "No active channel.\r\n";
			else
				std::cout << "Active channel: " << chan->name << "\r\n";
		}}});
		add({"info",  {0, 1, false, [&](sptr, line il) {
			if (il.args.size() == 0) {
				debug::print_all(pp);
				return;
			}
			
			const std::string &first = il.first();
			YIKES("Unknown option: " << first);
		}}});
	}

	server_ptr client::active_server() {
		return pp.active_server;
	}

	channel_ptr client::active_channel() {
		return pp.active_server? pp.active_server->active_channel : nullptr;
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
