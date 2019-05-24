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
	client & client::operator+=(const command_pair &p) {
		add(p);
		return *this;
	}
	
	client & client::operator+=(const server_ptr &ptr) {
		*pp += ptr;
		return *this;
	}

	void client::init() {
		add_listeners();
		add_handlers();
	}

	void client::input_worker() {
		std::string in;

		while (std::getline(std::cin, in)) {
			input_line il = input_line(in);

			if (il.is_command()) {
				try {
					if (!handle_line(il)) {
						std::cerr << "Unknown command: /" << il.command << std::endl;
					}
				} catch (std::exception &err) {
					YIKES("Caught an exception (" << typeid(err).name() << "): " << err.what());
				}
			} else if (channel_ptr chan = active_channel()) {
				privmsg_command(*chan, in).send(true);
				pp->dbgout() << "[" << *chan << "] <" << active_nick() << "> " << in << "\n";
			} else {
				YIKES("No active channel.");
			}
		}
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
			} else if (needs_serv && !pp->active_server) {
				YIKES("No server is selected.");
			} else {
				fn(pp->active_server, il);
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
				pp->dbgout() << std::string(*(ev->msg)) << "\n";
		});
	}

	void client::add_handlers() {
		using sptr = pingpong::server_ptr;
		using line = const input_line &;

		add<join_command>("join");

		add({"nick",  {0,  1, true, [](sptr serv, line il) {
			if (il.args.size() == 0)
				std::cout << "Current nick: " << serv->get_nick() << "\n";
			else
				nick_command(serv, il.first()).send(true);
		}}});
		add({"msg",   {2, -1, true, [](sptr serv, line il) { msg_command(serv, il.first(), il.rest()).send(true); }}});
		add({"quote", {1, -1, true, [](sptr serv, line il) { serv->quote(il.body);                                }}});
		add({"part",  {1, -1, true, [](sptr serv, line il) {
			const std::string &chan = il.first();
			if (!serv->has_channel(chan))
				YIKES(chan << ": no such channel.");
			else
				part_command(serv, chan, il.rest()).send(true);
		}}});
		add({"quit",  {0, -1, false, [&](sptr, line il) {
			if (il.args.empty()) {
				for (auto serv: pp->servers)
					quit_command(serv).send(true);
			} else {
				for (auto serv: pp->servers)
					quit_command(serv, il.body).send(true);
			}
		}}});
		add({"chans", {0, 0, true, [](sptr serv, line) {
			std::cout << "Channels:";
			for (auto [name, chan]: serv->channels)
				std::cout << " " << name;
			std::cout << "\n";
		}}});
		add({"chan",  {0, 0, true, [](sptr serv, line) {
			channel_ptr chan = serv->active_channel;
			if (chan == nullptr)
				std::cout << "No active channel.\n";
			else
				std::cout << "Active channel: " << chan->name << "\n";
		}}});
		add({"info",  {0, 1, false, [&](sptr, line il) {
			if (il.args.size() == 0) {
				debug::print_all(*pp);
				return;
			}
			
			const std::string &first = il.first();
			YIKES("Unknown option: " << first);
		}}});
	}

	void client::start_input() {
		input_thread = std::make_shared<std::thread>(&client::input_worker, this);
	}

	void client::join() {
		if (input_thread)
			input_thread->join();
	}

	server_ptr client::active_server() {
		return pp->active_server;
	}

	channel_ptr client::active_channel() {
		return pp->active_server? pp->active_server->active_channel : nullptr;
	}

	std::string client::active_nick() {
		if (server_ptr serv = active_server())
			return serv->get_nick();
		return std::string();
	}
}

int main(int argc, char **argv) {
	std::shared_ptr<irc> pp = irc::shared();
	pp->init();
	client instance(pp);
	instance.init();

	string hostname;

	hostname = 1 < argc? argv[1] : "localhost";
	std::shared_ptr<server> sserv = std::make_shared<server>(pp, hostname);
	server_ptr serv = sserv.get();
	serv->start();
	serv->set_nick("pingpong");
	instance += serv;
	instance.start_input();
	serv->server_thread->join();
	instance.join();
}
