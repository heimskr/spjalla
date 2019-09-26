#include <iostream>
#include <string>

#include "haunted/core/util.h"

#include "pingpong/core/channel.h"
#include "pingpong/core/debug.h"
#include "pingpong/core/ppdefs.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"

#include "pingpong/commands/mode.h"

#include "pingpong/net/resolution_error.h"

#include "core/client.h"
#include "core/input_line.h"

#include "lines/lines.h"

#include "formicine/ansi.h"

namespace spjalla {
	client::~client() {
		term.join();
	}


// Private instance methods


	void client::no_channel() {
		ui.log(lines::red_notice + "No active channel.");
	}

	std::string client::active_server_id() {
		return pp.active_server? pp.active_server->id : "";
	}

	std::string client::active_server_hostname() {
		return pp.active_server? pp.active_server->hostname : "";
	}


// Public instance methods


	client & client::operator+=(const command_pair &p) {
		add(p);
		return *this;
	}
	
	client & client::operator+=(pingpong::server *ptr) {
		pp += ptr;
		return *this;
	}

	void client::add(const command_pair &p) {
		command_handlers.insert(p);
	}

	void client::init() {
		term.watch_size();
		ui.start();
		pp.init();
		add_events();
		add_commands();
		term.start_input();
		add_input_listener();
		init_heartbeat();
	}

	void client::stop() {
		alive = false;
	}

	void client::server_removed(pingpong::server *serv) {
		// We need to check the windows in reverse because we're removing some along the way. Removing elements while
		// looping through a vector causes all kinds of problems unless you loop in reverse.
		std::vector<haunted::ui::control *> &windows = ui.swappo->get_children();
		for (auto iter = windows.rbegin(), rend = windows.rend(); iter != rend; ++iter) {
			ui::window *win = dynamic_cast<ui::window *>(*iter);
			if (win->data.serv == serv) {
				ui.remove_window(win);
			}
		}
	}

	void client::join() {
		term.join();
		if (heartbeat.joinable()) {
			heartbeat_alive = false;
			heartbeat.join();
		}
	}

	pingpong::server * client::active_server() {
		return pp.active_server;
	}

	std::string client::active_nick() {
		if (pingpong::server *serv = active_server())
			return serv->get_nick();
		return "";
	}
}
