#include <cstdlib>

#include "pingpong/core/debug.h"

#include "pingpong/commands/join.h"
#include "pingpong/commands/kick.h"
#include "pingpong/commands/mode.h"
#include "pingpong/commands/nick.h"
#include "pingpong/commands/privmsg.h"
#include "pingpong/commands/quit.h"
#include "pingpong/commands/whois.h"

#include "pingpong/net/resolution_error.h"

#include "spjalla/core/client.h"
#include "spjalla/core/util.h"

#include "spjalla/config/config.h"

#include "spjalla/lines/alias.h"
#include "spjalla/lines/config_group.h"
#include "spjalla/lines/config_key.h"
#include "spjalla/lines/line.h"
#include "spjalla/lines/warning.h"

#include "lib/formicine/futil.h"

#include "haunted/tests/test.h"

namespace spjalla {
	void client::add_commands() {
		using sptr = pingpong::server *;
		using line = const input_line &;

		pingpong::command::before_send = [&](pingpong::command &cmd) { return before_send(cmd); };

		add({"_args", {0, -1, false, [&](sptr, line il) {
			std::vector<std::string> strings {};
			ui.log("Body: " + "\""_d + util::escape(il.body) + "\""_d);
			DBG("Body: " + "\""_d + util::escape(il.body) + "\""_d);
			if (il.args.empty()) {
				ui.log("Args: " + "(none)"_d, ui.active_window);
				DBG("Args: " + "(none)"_d);
				return;
			}

			std::transform(il.args.begin(), il.args.end(), std::back_inserter(strings), [](const std::string &str) {
				return "\""_d + util::escape(str) + "\""_d;
			});
			std::string joined = "Args: " + formicine::util::join(strings.begin(), strings.end());
			ui.log(joined, ui.active_window);
			DBG(joined);
		}, {}}});


		add({"_dbg", {0, 0, false, [&](sptr, line) {
			debug_servers();
		}, {}}});


		// Handles a fake line of input as if the client had read it from a socket.
		add({"_fake", {0, -1, true, [&](sptr serv, line il) {
			serv->handle_line(pingpong::line(serv, il.body));
		}, {}}});


		add({"_info", {0, 1, false, [&](sptr, line il) {
			if (il.args.size() == 0) {
				pingpong::debug::print_all(irc);
				return;
			}

			const std::string &first = il.first();
			ui.log("Unknown option: " + first);
		}, {}}});


		add({"_time", {0, 0, false, [&](sptr, line) {
			DBG("Default time: " << pingpong::util::timestamp());
			DBG("Seconds:      " << pingpong::util::seconds());
			DBG("Milliseconds: " << pingpong::util::millistamp());
			DBG("Microseconds: " << pingpong::util::microstamp());
			DBG("Nanoseconds:  " << pingpong::util::nanostamp());
		}, {}}});


		add({"_win", {0, 0, false, [&](sptr, line) {
			if (ui::window *win = ui.active_window) {
				DBG("Window name:    " << ansi::bold(win->window_name));
				switch (win->type) {
					case ui::window_type::channel: DBG("Window type:    " << "channel"_b); break;
					case ui::window_type::user:    DBG("Window type:    " << "user"_b);    break;
					case ui::window_type::status:  DBG("Window type:    " << "status"_b);  break;
					case ui::window_type::overlay: DBG("Window type:    " << "overlay"_b); break;
					case ui::window_type::other:   DBG("Window type:    " << "other"_b);   break;
					default: DBG("Window type:    " << "invalid"_d);
				}
				DBG("Window user:    " << (win->user? win->user->name : "null"_d));
				DBG("Window channel: " << (win->chan? win->chan->name : "null"_d));
				DBG("Window server:  " << (win->serv? win->serv->id : "null"_d));
				switch (win->highest_notification) {
					case notification_type::none:      DBG("Window highest: " << "none"_b);      break;
					case notification_type::info:      DBG("Window highest: " << "info"_b);      break;
					case notification_type::message:   DBG("Window highest: " << "message"_b);   break;
					case notification_type::highlight: DBG("Window highest: " << "highlight"_b); break;
					default: DBG("Window highest: " << "invalid"_d);
				}
				DBG("Window voffset: " << win->get_voffset());
				DBG("Window autoscroll: " << (win->get_autoscroll()? "true" : "false"));
			} else {
				DBG("Window: " << "null"_d);
			}
		}, {}}});


		add({"alias", {0, -1, false, [&](sptr, line il) { commands::do_alias(*this, il); }, {}}});

		add({"ban", {1, 2, true, [&](sptr serv, line il) {
			ban(serv, il, "+b");
		}, {}}});

		add({"clear", {0, 0, false, [&](sptr, line) {
			if (ui::window *win = ui.get_active_window()) {
				win->set_voffset(win->total_rows());
			} else {
				DBG(lines::red_notice + "No window.");
			}
		}, {}}});

		add({"connect",    {1,  2, false, [&](sptr,      line il) { commands::do_connect(*this, il);          }, {}}});
		add({"disconnect", {0, -1, false, [&](sptr serv, line il) { commands::do_disconnect(*this, serv, il); }, {}}});
		add({"join",       {1,  1, true,  [&](sptr serv, line il) { commands::do_join(*this, serv, il);       }, {}}});

		add({"kick", {1, -1, true, [&](sptr serv, line il) {
			if (triple_command<pingpong::kick_command>(serv, il, ui.get_active_channel()))
				no_channel();
		}, {}}});

		add({"me",   {1, -1, true,  [&](sptr,      line il) { commands::do_me(ui, il);   }, completions::complete_me}});
		add({"mode", {1, -1, true,  [&](sptr serv, line il) { commands::do_mode(ui, serv, il); }, {}}});
		add({"move", {1,  1, false, [&](sptr,      line il) { commands::do_move(ui, il); }, {}}});

		add({"msg", {2, -1, true, [&](sptr serv, line il) {
			pingpong::privmsg_command(serv, il.first(), il.rest()).send();
		}, {}}});

		add({"nick",    {0,  1, true,  [&](sptr serv, line il) { commands::do_nick(ui, serv, il);    }, {}}});
		add({"overlay", {0,  0, false, [&](sptr,      line)    { ui.update_overlay();                }, {}}});
		add({"part",    {0, -1, true,  [&](sptr serv, line il) { commands::do_part(*this, serv, il); }, {}}});

		add({"quit", {0, -1, false, [&](sptr, line il) {
			for (pingpong::server *serv: irc.server_order)
				serv->quit(il.body);
		}, {}}});

		add({"quote", {1, -1, true, [&](sptr serv, line il) {
			serv->quote(il.body);
		}, {}}});

		add({"set",   {0, -1, false, [&](sptr, line il) { commands::do_set(*this, il); }, completions::complete_set}});
		add({"spam",  {0,  1, false, [&](sptr, line il) { commands::do_spam(ui, il); }, {}}});
		add({"topic", {0, -1, true,  [&](sptr serv, line il) { commands::do_topic(*this, serv, il); }, {}}});

		add({"unban", {1, 2, true, [&](sptr serv, line il) {
			ban(serv, il, "-b");
		}, {}}});

		add({"wc", {0, 0, false, [&](sptr, line) {
			if (ui.can_remove())
				ui.remove_window(ui.active_window);
		}, {}}});

		add({"whois", {1, 1, true, [&](sptr serv, line il) {
			pingpong::whois_command(serv, il.first()).send();
		}, completions::complete_plain}});
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

	void client::debug_servers() {
		if (irc.servers.empty()) {
			DBG("No servers.");
			return;
		}

		for (const auto &pair: irc.servers) {
			pingpong::server *serv = pair.second;
			DBG(ansi::bold(serv->id) << " (" << serv->get_nick() << "@"_d << serv->hostname << "): "
				<< static_cast<int>(serv->status));
			for (std::shared_ptr<pingpong::channel> chan: serv->channels) {
				DBG("    " << ansi::wrap(chan->name, ansi::style::underline) << " [" << chan->mode_str() << "]");
				for (std::shared_ptr<pingpong::user> user: chan->users) {
					std::string chans = "";
					std::vector<std::weak_ptr<pingpong::channel>> expired_chans {};
					for (std::weak_ptr<pingpong::channel> user_chan: user->channels) {
						if (user_chan.expired()) {
							expired_chans.push_back(user_chan);
						} else {
							chans += " " + user_chan.lock()->name;
						}
					}

					for (const std::weak_ptr<pingpong::channel> &expired_chan: expired_chans)
						user->channels.erase(expired_chan);

					if (chans.empty())
						DBG("        " << user->name);
					else
						DBG("        " << user->name << ":" << chans);
				}
			}
		}
	}
}
