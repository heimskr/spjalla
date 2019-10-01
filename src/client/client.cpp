#include <iostream>
#include <string>

#include "haunted/core/util.h"

#include "pingpong/core/channel.h"
#include "pingpong/core/debug.h"
#include "pingpong/core/defs.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"

#include "pingpong/commands/mode.h"

#include "pingpong/net/resolution_error.h"

#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"

#include "spjalla/lines/line.h"

#include "lib/formicine/ansi.h"

namespace spjalla {
	client::client(int heartbeat_period_): out_stream(ansi::out), term(haunted::terminal(std::cin, out_stream)),
	ui(&term, this), completer(*this), configs({*this, false}), heartbeat_period(heartbeat_period_) {
		config::register_defaults();
		term.key_postlistener = [&](const haunted::key &k) { key_postlistener(k); };
	}

	client::~client() {
		term.join();
	}


// Private instance methods


	void client::no_channel() {
		ui.log(lines::red_notice + "No active channel.");
	}


// Public instance methods


	std::string client::active_server_id() {
		return irc.active_server? irc.active_server->id : "";
	}

	std::string client::active_server_hostname() {
		return irc.active_server? irc.active_server->hostname : "";
	}


	client & client::operator+=(const spjalla::commands::pair &p) {
		add(p);
		return *this;
	}
	
	client & client::operator+=(pingpong::server *ptr) {
		irc += ptr;
		return *this;
	}

	void client::add(const spjalla::commands::pair &p) {
		command_handlers.insert(p);
		completion_states.insert({p.first, completions::completion_state(p.second.suggestors)});
	}

	void client::init() {
		configs.read_if_empty(true);
		ui.start();
		irc.init();
		add_events();
		add_commands();
		add_input_listener();
		term.start_input();
		init_heartbeat();
		init_statusbar();
	}

	void client::server_removed(pingpong::server *serv) {
		call_in_queue(serv, pingpong::server::stage::dead);
		server_status_queue.erase(serv);

		// We need to check the windows in reverse because we're removing some along the way. Removing elements while
		// looping through a vector causes all kinds of problems unless you loop in reverse.
		haunted::ui::container::type &windows = ui.swappo->get_children();
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
		return irc.active_server;
	}

	std::string client::active_nick() {
		if (pingpong::server *serv = active_server())
			return serv->get_nick();
		return "";
	}

	std::deque<haunted::ui::control *> client::get_window_controls() const {
		return ui.swappo->get_children();
	}
}
