#include <iostream>
#include <string>

#include "spjalla/core/options.h"

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
#include "lib/formicine/performance.h"

namespace spjalla {
	client::client(int heartbeat_period_): out_stream(ansi::out), term(haunted::terminal(std::cin, out_stream)),
	ui(term, *this), completer(*this), configs({*this, false}), heartbeat_period(heartbeat_period_) {
		config::register_defaults();
		DBG("Set up configs.");
		term.key_postlistener = [&](const haunted::key &k) { key_postlistener(k); };
	}

	client::~client() {
		status_widgets.clear();
		heartbeat_listeners.clear();
		command_handlers.clear();
		close_plugins();
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

	void client::add(const std::string &command_name, const commands::command &command) {
		add({command_name, command});
	}

	void client::add(const std::string &command_name, int min_args, int max_args, bool needs_server,
	                 const commands::command::handler_fn &handler_fn,
	                 const completions::completion_fn &completion_fn,
	                 const std::vector<completions::completion_state::suggestor_fn> &suggestors) {
		add({command_name, {min_args, max_args, needs_server, handler_fn, completion_fn, suggestors}});
	}

	void client::init() {
		configs.read_if_empty(DEFAULT_CONFIG_DB, true);
		alias_db.read_if_empty(DEFAULT_ALIAS_DB, true);
		ui.init();
		irc.init();
		add_events();
		add_commands();
		add_input_listener();
		term.start_input();
		init_heartbeat();
		init_statusbar();
	}

	void client::postinit() {
		ui.postinit();
	}

	void client::server_removed(pingpong::server *serv) {
		call_in_queue(serv, pingpong::server::stage::dead);
		server_status_queue.erase(serv);

		// We need to check the windows in reverse because we're removing some along the way. Removing elements while
		// looping through a vector causes all kinds of problems unless you loop in reverse.
		haunted::ui::container::type &windows = ui.swappo->get_children();
		for (auto iter = windows.rbegin(), rend = windows.rend(); iter != rend; ++iter) {
			ui::window *win = dynamic_cast<ui::window *>(*iter);
			if (win->serv == serv) {
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

	// void client::cleanup() {
	// 	plugin_host::cleanup();
	// 	command_handlers.clear();
	// 	heartbeat_listeners.clear();
	// 	status_widgets.clear();
	// 	join();
	// 	close_plugins();
	// 	term.join();
	// 	formicine::perf.results();
	// }

	pingpong::server * client::active_server() {
		ui::window *win = ui.active_window;
		if (win && win->serv)
			return win->serv;
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

	ui::window * client::query(const std::string &name, pingpong::server *serv) {
		if (!serv)
			serv = active_server();

		if (!serv)
			throw std::runtime_error("Can't query user: server is null");
		return query(serv->get_user(name, true, false));
	}

	ui::window * client::query(std::shared_ptr<pingpong::user> whom) {
		return ui.get_window(whom, true);
	}

	void client::no_channel() {
		ui.log(lines::red_notice + "No active channel.");
	}
}
