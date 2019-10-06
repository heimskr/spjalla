#include "pingpong/commands/quit.h"

#include "pingpong/events/bad_line.h"
#include "pingpong/events/command.h"
#include "pingpong/events/error.h"
#include "pingpong/events/hats_updated.h"
#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/message.h"
#include "pingpong/events/mode.h"
#include "pingpong/events/names_updated.h"
#include "pingpong/events/nick.h"
#include "pingpong/events/part.h"
#include "pingpong/events/privmsg.h"
#include "pingpong/events/quit.h"
#include "pingpong/events/raw.h"
#include "pingpong/events/server_status.h"
#include "pingpong/events/topic.h"
#include "pingpong/events/topic_updated.h"
#include "pingpong/events/user_appeared.h"

#include "pingpong/messages/join.h"
#include "pingpong/messages/numeric.h"
#include "pingpong/messages/ping.h"

#include "spjalla/core/client.h"

#include "spjalla/lines/basic.h"
#include "spjalla/lines/join.h"
#include "spjalla/lines/kick.h"
#include "spjalla/lines/mode.h"
#include "spjalla/lines/nick_change.h"
#include "spjalla/lines/part.h"
#include "spjalla/lines/privmsg.h"
#include "spjalla/lines/quit.h"
#include "spjalla/lines/topic.h"

namespace spjalla {
	void client::add_events() {
		pingpong::events::listen<pingpong::bad_line_event>([&](pingpong::bad_line_event *ev) {
			ui.log(lines::basic_line(ansi::wrap(">> ", ansi::color::red) + ev->bad_line, 3));
		});

		pingpong::events::listen<pingpong::command_event>([&](pingpong::command_event *ev) {
			if (pingpong::quit_command *quit = dynamic_cast<pingpong::quit_command *>(ev->cmd))
				server_removed(quit->serv);
		});

		pingpong::events::listen<pingpong::error_event>([&](pingpong::error_event *ev) {
			ui::window *win = ev->current_window? ui.active_window : ui.status_window;
			*win += lines::basic_line(lines::red_notice + ev->message, ansi::length(lines::red_notice));
		});

		pingpong::events::listen<pingpong::hats_updated_event>([&](pingpong::hats_updated_event *ev) {
			if (ui.is_active(ui.get_window(ev->chan, false))) {
				ui.update_overlay(ev->chan);
				ui.update_statusbar();
			}
		});

		pingpong::events::listen<pingpong::join_event>([&](pingpong::join_event *ev) {
			ui::window *win = ui.get_window(ev->chan, true);
			*win += lines::join_line(*ev);

			if (ev->who->is_self()) {
				win->resurrect();
				ui.focus_window(win);
			}
		});

		pingpong::events::listen<pingpong::kick_event>([&](pingpong::kick_event *ev) {
			if (ui::window *win = ui.get_window(ev->chan, false)) {
				if (ev->whom->is_self()) {
					win->kill();
					if (win == ui.active_window) {
						ui.update_statusbar();
						ui.update_titlebar();
					}
				}

				*win += lines::kick_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::mode_event>([&](pingpong::mode_event *ev) {
			lines::mode_line mline {*ev};

			ui::window *win = nullptr;
			if (ev->is_channel())
				win = try_window(ev->get_channel(ev->serv));

			ui.log(mline, win);
		});

		pingpong::events::listen<pingpong::names_updated_event>([&](pingpong::names_updated_event *ev) {
			if (ui::window *win = ui.get_active_window()) {
				if (win->chan == ev->chan)
					ui.update_overlay();
			}
		});

		pingpong::events::listen<pingpong::nick_event>([&](pingpong::nick_event *ev) {
			lines::nick_change_line nline {*ev};
			for (ui::window *win: ui.windows_for_user(ev->who))
				*win += nline;
		});

		pingpong::events::listen<pingpong::part_event>([&](pingpong::part_event *ev) {
			if (ui::window *win = try_window(ev->chan)) {
				*win += lines::part_line(*ev);
				if (ev->who->is_self())
					win->kill();
			}
		});

		pingpong::events::listen<pingpong::privmsg_event>([&](pingpong::privmsg_event *ev) {
			const bool direct_only = configs.get("activity", "direct_only").bool_();
			if (ev->is_channel()) {
				*ui.get_window(ev->get_channel(ev->serv), true) += lines::privmsg_line(*ev, direct_only);
			} else {
				if (ev->speaker->is_self()) // privmsg_events are dispatched when we send messages too.
					*ui.get_window(ev->serv->get_user(ev->where, true), true) += lines::privmsg_line(*ev, direct_only);
				else
					*ui.get_window(ev->speaker, true) += lines::privmsg_line(*ev, direct_only);
			}
		});

		pingpong::events::listen<pingpong::quit_event>([&](pingpong::quit_event *ev) {
			std::shared_ptr<pingpong::user> who = ev->who;
			lines::quit_line qline = lines::quit_line(who, ev->content, ev->stamp);
			for (ui::window *win: ui.windows_for_user(who)) {
				*win += qline;
				if (win->user == who) {
					win->kill();
					if (win == ui.active_window) {
						ui.update_statusbar();
						ui.update_titlebar();
					}
				}
			}
		});

		pingpong::events::listen<pingpong::raw_in_event>([&](pingpong::raw_in_event *ev) {
			ui.log(lines::basic_line(ansi::wrap("<< ", ansi::color::gray) + ev->raw_in, 3));
		});

		pingpong::events::listen<pingpong::raw_out_event>([&](pingpong::raw_out_event *ev) {
			ui.log(lines::basic_line(ansi::wrap(">> ", ansi::color::lightgray) + ev->raw_out, 3));
		});

		pingpong::events::listen<pingpong::server_status_event>([&](pingpong::server_status_event *ev) {
			call_in_queue(ev->serv, ev->serv->get_status());

			ui.update_statusbar();
			if (ui.active_window == ui.overlay)
				ui.update_overlay();
		});

		pingpong::events::listen<pingpong::topic_event>([&](pingpong::topic_event *ev) {
			if (ui::window *win = try_window(ev->chan))
				*win += lines::topic_line(*ev);
		});

		pingpong::events::listen<pingpong::topic_updated_event>([&](pingpong::topic_updated_event *ev) {
			if (ui.get_active_channel() == ev->chan)
				ui.update_titlebar(ev->chan);
		});

		pingpong::events::listen<pingpong::user_appeared_event>([&](pingpong::user_appeared_event *ev) {
			for (ui::window *win: ui.windows_for_user(ev->who)) {
				win->resurrect();
				if (win == ui.active_window) {
					ui.update_statusbar();
					ui.update_titlebar();
				}
			}
		});
	}

	void client::call_in_queue(pingpong::server *server, pingpong::server::stage stage) {
		if (server_status_queue.count(server) == 0)
			return;

		std::list<queue_pair> &list = server_status_queue.at(server);
		for (auto iter = list.begin(), end = list.end(); iter != end; ++iter) {
			const pingpong::server::stage requested_stage = iter->first;
			const queue_fn &fn = iter->second;

			if (requested_stage == stage) {
				fn();
				list.erase(iter++);
			}
		}
	}

	void client::wait_for_server(pingpong::server *server, pingpong::server::stage stage, const client::queue_fn &fn) {
		if (server->get_status() == stage) {
			fn();
		} else {
			server_status_queue[server].push_back({stage, fn});
		}
	}
}
