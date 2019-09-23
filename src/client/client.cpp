#include <iostream>
#include <string>
#include <thread>

#include "haunted/core/util.h"

#include "pingpong/core/channel.h"
#include "pingpong/core/debug.h"
#include "pingpong/core/ppdefs.h"
#include "pingpong/core/irc.h"
#include "pingpong/core/server.h"

#include "pingpong/commands/mode.h"
#include "pingpong/commands/privmsg.h"

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


	void client::debug_servers() {
		if (pp.servers.empty()) {
			DBG("No servers.");
			return;
		}

		for (const auto &pair: pp.servers) {
			pingpong::server *serv = pair.second;
			DBG(ansi::bold(serv->hostname));
			for (std::shared_ptr<pingpong::channel> chan: serv->channels) {
				DBG("    " << ansi::wrap(chan->name, ansi::style::underline) << " [" << chan->mode_str() << "]");
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

	void client::no_channel() {
		ui.log(lines::red_notice + "No active channel.");
	}

	std::string client::active_server_name() {
		return pp.active_server? pp.active_server->hostname : "none";
	}

	void client::ban(pingpong::server *serv, const input_line &il, const std::string &type) {
		std::shared_ptr<pingpong::channel> chan = ui.get_active_channel();
		std::string target;

		if (il.args.size() == 2) {
			if (il.args[0].front() != '#') {
				ui.warn("Invalid channel name: " + il.args[0]);
				return;
			}

			chan = serv->get_channel(il.args[0], false);
			if (!chan) {
				ui.warn("Channel not found: " + il.args[0]);
				return;
			}

			target = il.args[1];
		} else {
			target = il.args[0];
		}

		if (!chan) {
			ui.warn("Cannot ban: no channel specified.");
			return;
		}

		pingpong::mode_command(chan, type, target).send();
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
			} else if (!ui.active_window->is_dead()) {
				if (std::shared_ptr<pingpong::channel> chan = ui.get_active_channel()) {
					pingpong::privmsg_command(chan, str).send();
				} else if (std::shared_ptr<pingpong::user> user = ui.get_active_user()) {
					pingpong::privmsg_command(user, str).send();
				} else {
					no_channel();
				}
			}
		});
	}

	void client::stop() {
		alive = false;
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
