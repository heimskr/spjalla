#include <iostream>
#include <string>
#include <thread>

#include "commands/all.h"
#include "core/debug.h"
#include "core/defs.h"
#include "core/irc.h"
#include "core/server.h"
#include "lib/ansi.h"
#include "events/event.h"
#include "events/message.h"
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

	void client::init() {
		add_listeners();
		add_handlers();
	}

	void client::input_worker(server_ptr serv) {
		std::string in;

		while (std::getline(std::cin, in)) {
			input_line il = input_line(in);

			if (il.is_command()) {
				try {
					if (!handle_line(serv, il)) {
						std::cerr << "Unknown command: /" << il.command << std::endl;
					}
				} catch (std::exception &err) {
					YIKES("Caught an exception (" << typeid(err).name() << "): " << err.what());
				}
			} else {
				
			}
		}
	}

	bool client::handle_line(server_ptr serv, const input_line &il) {
		const int nargs = static_cast<int>(il.args.size());
		const std::string &name = il.command;

		auto range = command_handlers.equal_range(name);
		if (range.first == range.second)
			return false;

		for (auto it = range.first; it != range.second; ++it) {
			auto [min, max, fn] = it->second;
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
			} else {
				fn(serv, il);
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

		add<nick_command>("nick");
		add<join_command>("join");

		add({"msg",   {2, -1, [](sptr serv, line il) { msg_command(serv, il.args[0], il.rest()).send(true);  }}});
		add({"quote", {1, -1, [](sptr serv, line il) { serv->quote(il.body);                                 }}});
		add({"part",  {1, -1, [](sptr serv, line il) { part_command(serv, il.args[0], il.rest()).send(true); }}});
		add({"quit",  {0, -1, [](sptr serv, line il) {
			(il.args.size() == 0? quit_command(serv) : quit_command(serv, il.body)).send(true);
		}}});
		add({"chans", {0, 0, [](sptr serv, line) {
			std::cout << "Channels:";
			for (const channel &chan: serv->channels)
				std::cout << " " << std::string(chan);
			std::cout << "\n";
		}}});
	}
}

int main(int argc, char **argv) {
	std::shared_ptr<irc> pp = irc::shared();
	pp->init();
	client instance(pp);
	instance.init();

	string hostname;

	hostname = 1 < argc? argv[1] : "localhost";
	server serv(pp, hostname);
	serv.start();
	serv.set_nick("pingpong");
	std::thread input(&client::input_worker, &instance, &serv);
	serv.server_thread->join();
	input.join();
}
