#include "pingpong/commands/Notice.h"
#include "pingpong/commands/Quit.h"

#include "pingpong/events/BadLine.h"
#include "pingpong/events/Command.h"
#include "pingpong/events/Error.h"
#include "pingpong/events/HatsUpdated.h"
#include "pingpong/events/Join.h"
#include "pingpong/events/Kick.h"
#include "pingpong/events/Message.h"
#include "pingpong/events/Mode.h"
#include "pingpong/events/Motd.h"
#include "pingpong/events/NamesUpdated.h"
#include "pingpong/events/Nick.h"
#include "pingpong/events/NickInUse.h"
#include "pingpong/events/Notice.h"
#include "pingpong/events/Part.h"
#include "pingpong/events/Privmsg.h"
#include "pingpong/events/Quit.h"
#include "pingpong/events/Raw.h"
#include "pingpong/events/ServerStatus.h"
#include "pingpong/events/Topic.h"
#include "pingpong/events/TopicUpdated.h"
#include "pingpong/events/UserAppeared.h"
#include "pingpong/events/VersionRequested.h"

#include "pingpong/messages/Join.h"
#include "pingpong/messages/Numeric.h"
#include "pingpong/messages/Ping.h"

#include "spjalla/core/Client.h"

#include "spjalla/lines/Basic.h"
#include "spjalla/lines/Error.h"
#include "spjalla/lines/Join.h"
#include "spjalla/lines/Kick.h"
#include "spjalla/lines/Mode.h"
#include "spjalla/lines/Motd.h"
#include "spjalla/lines/NickChange.h"
#include "spjalla/lines/Notice.h"
#include "spjalla/lines/Part.h"
#include "spjalla/lines/Privmsg.h"
#include "spjalla/lines/Raw.h"
#include "spjalla/lines/Quit.h"
#include "spjalla/lines/ServerBasic.h"
#include "spjalla/lines/Topic.h"

namespace Spjalla {
	void Client::addEvents() {
		PingPong::Events::listen<PingPong::BadLineEvent>([&](PingPong::BadLineEvent *ev) {
			ui.log(Lines::RawLine(this, ev->badLine, ev->server, false, true, ev->stamp));
		});

		PingPong::Events::listen<PingPong::CommandEvent>([&](PingPong::CommandEvent *ev) {
			if (PingPong::QuitCommand *quit = dynamic_cast<PingPong::QuitCommand *>(ev->command))
				serverRemoved(quit->server);
		});

		PingPong::Events::listen<PingPong::ErrorEvent>([&](PingPong::ErrorEvent *ev) {
			UI::Window *win = ev->currentWindow? ui.activeWindow : ui.statusWindow;
			*win += Lines::BasicLine(this, Lines::redNotice + ev->message, ansi::length(Lines::redNotice));
		});

		PingPong::Events::listen<PingPong::HatsUpdatedEvent>([&](PingPong::HatsUpdatedEvent *ev) {
			if (ui.isActive(ui.getWindow(ev->channel, false))) {
				ui.updateOverlay(ev->channel);
				ui.updateStatusbar();
			}
		});

		PingPong::Events::listen<PingPong::JoinEvent>([&](PingPong::JoinEvent *ev) {
			UI::Window *window = ui.getWindow(ev->channel, true);
			*window += Lines::JoinLine(this, *ev);

			if (ev->who->isSelf()) {
				window->resurrect();
				ui.focusWindow(window);
				ui.updateStatusbar();
			}
		});

		PingPong::Events::listen<PingPong::KickEvent>([&](PingPong::KickEvent *ev) {
			if (UI::Window *window = ui.getWindow(ev->channel, false)) {
				if (ev->whom->isSelf()) {
					window->kill();
					if (window == ui.activeWindow) {
						ui.updateStatusbar();
						ui.updateTitlebar();
					}
				}

				*window += Lines::KickLine(this, *ev);
			}
		});

		PingPong::Events::listen<PingPong::ModeEvent>([&](PingPong::ModeEvent *ev) {
			Lines::ModeLine mline {this, *ev};

			UI::Window *window = nullptr;
			if (ev->isChannel())
				window = tryWindow(ev->getChannel(ev->server));

			ui.log(mline, window);
		});

		PingPong::Events::listen<PingPong::MotdEvent>([&](PingPong::MotdEvent *ev) {
			if (cache.interfaceShowMotds)
				ui.log(Lines::MotdLine(this, ev->content, ev->server, ev->stamp));
		});

		PingPong::Events::listen<PingPong::NamesUpdatedEvent>([&](PingPong::NamesUpdatedEvent *ev) {
			if (UI::Window *window = ui.getActiveWindow()) {
				if (window->channel == ev->channel)
					ui.updateOverlay();
			}
		});

		PingPong::Events::listen<PingPong::NickEvent>([&](PingPong::NickEvent *ev) {
			Lines::NickChangeLine nline(this, *ev);
			for (UI::Window *window: ui.windowsForUser(ev->who))
				*window += nline;
			if (ev->who->isSelf())
				*ui.statusWindow += Lines::ServerBasicLine(this, ev->server,
					ui.renderer("self_nick_change", {{"raw_new", ev->who->name}}));
		});

		PingPong::Events::listen<PingPong::NickInUseEvent>([&](PingPong::NickInUseEvent *ev) {
			ui.warn("Nick in use: " + ansi::bold(ev->nick), ev->server);
		});

		PingPong::Events::listen<PingPong::NoticeEvent>([&](PingPong::NoticeEvent *ev) {
			const bool direct_only       = configs.get("messages", "direct_only").bool_();
			const bool highlight_notices = configs.get("messages", "highlight_notices").bool_();
			const bool in_status         = configs.get("messages", "notices_in_status").bool_();

			if (!ev->isChannel() && !ev->speaker) {
				UI::Window *window = in_status? ui.statusWindow : ui.activeWindow;
				Lines::NoticeLine nline(this, ev->server->id, "*", ev->server->getNick(), ev->content, ev->stamp, {},
					true);
				nline.server = ev->server;
				*window += std::move(nline);
				return;
			}

			Lines::NoticeLine nline = {this, *ev, direct_only, highlight_notices};
			if (ev->isChannel()) {
				*ui.getWindow(ev->getChannel(ev->server), true) += nline;
			} else if (ev->speaker && ev->speaker->isSelf()) {
				*ui.getWindow(ev->server->getUser(ev->where, true, true), true) += nline;
			} else if (in_status) {
				*ui.statusWindow += nline;
			} else {
				*ui.getWindow(ev->speaker, true) += nline;
			}
		});

		PingPong::Events::listen<PingPong::PartEvent>([&](PingPong::PartEvent *ev) {
			if (UI::Window *win = tryWindow(ev->channel)) {
				*win += Lines::PartLine(this, *ev);
				if (ev->who->isSelf()) {
					win->kill();
					if (configs.get("interface", "close_on_part").bool_())
						ui.removeWindow(win);
					ui.updateStatusbar();
				}
			}
		});

		PingPong::Events::listen<PingPong::PrivmsgEvent>([&](PingPong::PrivmsgEvent *ev) {
			const bool direct_only = configs.get("messages", "direct_only").bool_();
			if (ev->isChannel()) {
				*ui.getWindow(ev->getChannel(ev->server), true) += Lines::PrivmsgLine(this, *ev, direct_only);
			} else {
				if (ev->speaker->isSelf()) { // privmsg_events are dispatched when we send messages too.
					Lines::PrivmsgLine privline(this, *ev, direct_only);
					privline.server = ev->server;
					*ui.getWindow(ev->server->getUser(ev->where, true, true), true) += privline;
				} else {
					*ui.getWindow(ev->speaker, true) += Lines::PrivmsgLine(this, *ev, direct_only);
				}
			}
		});

		PingPong::Events::listen<PingPong::QuitEvent>([&](PingPong::QuitEvent *ev) {
			std::shared_ptr<PingPong::User> who = ev->who;
			Lines::QuitLine qline(this, who, ev->content, ev->stamp);
			for (UI::Window *win: ui.windowsForUser(who)) {
				*win += qline;
				if (win->user == who) {
					win->kill();
					if (win == ui.activeWindow) {
						ui.updateStatusbar();
						ui.updateTitlebar();
					}
				}
			}
		});

		PingPong::Events::listen<PingPong::RawInEvent>([&](PingPong::RawInEvent *ev) {
			if (cache.debugShowRaw)
				ui.log(Lines::RawLine(this, ev->rawIn, ev->server, false, false, ev->stamp));
		});

		PingPong::Events::listen<PingPong::RawOutEvent>([&](PingPong::RawOutEvent *ev) {
			if (cache.debugShowRaw)
				ui.log(Lines::RawLine(this, ev->rawOut, ev->server, true, false, ev->stamp));
		});

		PingPong::Events::listen<PingPong::ServerStatusEvent>([&](PingPong::ServerStatusEvent *ev) {
			callInQueue(ev->server, ev->server->getStatus());

			if (ev->server->getStatus() == PingPong::Server::Stage::Ready)
				ui.log("Connected to " + ansi::bold(ev->server->id) + ".");

			ui.updateStatusbar();
			if (ui.activeWindow == ui.overlay)
				ui.updateOverlay();
		});

		PingPong::Events::listen<PingPong::TopicEvent>([&](PingPong::TopicEvent *ev) {
			if (UI::Window *window = tryWindow(ev->channel))
				*window += Lines::TopicLine(this, *ev);
		});

		PingPong::Events::listen<PingPong::TopicUpdatedEvent>([&](PingPong::TopicUpdatedEvent *ev) {
			if (ui.getActiveChannel() == ev->channel)
				ui.updateTitlebar(ev->channel);
		});

		PingPong::Events::listen<PingPong::UserAppearedEvent>([&](PingPong::UserAppearedEvent *ev) {
			for (UI::Window *window: ui.windowsForUser(ev->who)) {
				window->resurrect();
				if (window == ui.activeWindow) {
					ui.updateStatusbar();
					ui.updateTitlebar();
				}
			}
		});

		PingPong::Events::listen<PingPong::VersionRequestedEvent>([&](PingPong::VersionRequestedEvent *ev) {
			const bool hide   = configs.get("behavior", "hide_version_requests").bool_();
			const bool answer = configs.get("behavior", "answer_version_requests").bool_();

			if (!hide)
				PingPong::Events::dispatch<PingPong::PrivmsgEvent>(ev->requester, ev->where, ev->content);

			if (answer)
				PingPong::NoticeCommand(ev->requester, "\x01VERSION " + irc.version + "\x01", true).send();
		});
	}

	void Client::callInQueue(PingPong::Server *server, PingPong::Server::Stage stage) {
		if (serverStatusQueue.count(server) == 0)
			return;

		std::list<QueuePair> &list = serverStatusQueue.at(server);
		for (auto iter = list.begin(), end = list.end(); iter != end;) {
			const PingPong::Server::Stage requested_stage = iter->first;
			const Queue_f &fn = iter->second;

			if (requested_stage == stage) {
				fn();
				list.erase(iter++);
			} else
				++iter;
		}
	}

	void Client::waitForServer(PingPong::Server *server, PingPong::Server::Stage stage, const Client::Queue_f &fn) {
		if (server->getStatus() == stage) {
			fn();
		} else {
			serverStatusQueue[server].push_back({stage, fn});
		}
	}
}
