#include "pingpong/commands/quit.h"

#include "pingpong/events/bad_line.h"
#include "pingpong/events/command.h"
#include "pingpong/events/error.h"
#include "pingpong/events/join.h"
#include "pingpong/events/kick.h"
#include "pingpong/events/message.h"
#include "pingpong/events/names_updated.h"
#include "pingpong/events/nick.h"
#include "pingpong/events/part.h"
#include "pingpong/events/privmsg.h"
#include "pingpong/events/quit.h"
#include "pingpong/events/raw.h"
#include "pingpong/events/server_status.h"

#include "pingpong/messages/join.h"
#include "pingpong/messages/numeric.h"
#include "pingpong/messages/ping.h"

#include "core/client.h"

#include "lines/join.h"
#include "lines/kick.h"
#include "lines/nick_change.h"
#include "lines/part.h"
#include "lines/privmsg.h"
#include "lines/quit.h"

namespace spjalla {
	void client::add_events() {
		pingpong::events::listen<pingpong::bad_line_event>([&](pingpong::bad_line_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::red) + ev->bad_line, 3));
		});

		pingpong::events::listen<pingpong::command_event>([&](pingpong::command_event *ev) {
			if (pingpong::quit_command *quit = dynamic_cast<pingpong::quit_command *>(ev->cmd))
				server_removed(quit->serv);
		});

		pingpong::events::listen<pingpong::error_event>([&](pingpong::error_event *ev) {
			ui::window *win = ev->current_window? ui.active_window : ui.status_window;
			*win += haunted::ui::simpleline(lines::red_notice + ev->message, ansi::length(lines::red_notice));
		});

		pingpong::events::listen<pingpong::join_event>([&](pingpong::join_event *ev) {
			ui::window *win = ui.get_window(ev->chan, true);
			*win += lines::join_line(*ev);

			if (ev->who->is_self()) {
				ui.focus_window(win);
				win->data.dead = false;
			}
		});

		pingpong::events::listen<pingpong::kick_event>([&](pingpong::kick_event *ev) {
			if (ui::window *win = ui.get_window(ev->chan, false)) {
				win->data.dead = true;
				*win += lines::kick_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::message_event>([&](pingpong::message_event *ev) {
			if (!ev->msg->is<pingpong::numeric_message>() && !ev->msg->is<pingpong::ping_message>())
				ui.log(*(ev->msg));
		});

		pingpong::events::listen<pingpong::names_updated_event>([&](pingpong::names_updated_event *ev) {
			if (ui::window *win = ui.get_active_window()) {
				if (win->data.chan == ev->chan)
					ui.update_overlay();
			}
		});

		pingpong::events::listen<pingpong::nick_event>([&](pingpong::nick_event *ev) {
			lines::nick_change_line nline = lines::nick_change_line(*ev);
			for (ui::window *win: ui.windows_for_user(ev->who))
				*win += nline;
		});

		pingpong::events::listen<pingpong::part_event>([&](pingpong::part_event *ev) {
			if (ui::window *win = try_window(ev->chan)) {
				*win += lines::part_line(*ev);
				if (ev->who->is_self())
					win->data.dead = true;
			}
		});

		pingpong::events::listen<pingpong::privmsg_event>([&](pingpong::privmsg_event *ev) {
			if (ev->is_channel()) {
				*ui.get_window(ev->get_channel(ev->serv), true) += lines::privmsg_line(*ev);
			} else {
				if (ev->speaker->is_self()) // privmsg_events are dispatched when we send messages too.
					*ui.get_window(ev->serv->get_user(ev->where, true), true) += lines::privmsg_line(*ev);
				else
					*ui.get_window(ev->speaker, true) += lines::privmsg_line(*ev);
			}
		});

		pingpong::events::listen<pingpong::quit_event>([&](pingpong::quit_event *ev) {
			lines::quit_line qline = lines::quit_line(ev->who, ev->content, ev->stamp);
			for (ui::window *win: ui.windows_for_user(ev->who))
				*win += qline;
		});

		pingpong::events::listen<pingpong::raw_in_event>([&](pingpong::raw_in_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap("<< ", ansi::color::gray) + ev->raw_in, 3));
		});

		pingpong::events::listen<pingpong::raw_out_event>([&](pingpong::raw_out_event *ev) {
			ui.log(haunted::ui::simpleline(ansi::wrap(">> ", ansi::color::lightgray) + ev->raw_out, 3));
		});

		pingpong::events::listen<pingpong::server_status_event>([&](pingpong::server_status_event *) {
			ui.update_statusbar();
			if (ui.active_window == ui.overlay)
				ui.update_overlay();
		});
	}
}