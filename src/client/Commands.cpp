#include <cstdlib>

#include "pingpong/core/Debug.h"

#include "pingpong/commands/Join.h"
#include "pingpong/commands/Kick.h"
#include "pingpong/commands/Mode.h"
#include "pingpong/commands/Nick.h"
#include "pingpong/commands/Privmsg.h"
#include "pingpong/commands/Quit.h"
#include "pingpong/commands/Whois.h"

#include "pingpong/net/ResolutionError.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/Util.h"

#include "spjalla/config/Config.h"

#include "spjalla/lines/Alias.h"
#include "spjalla/lines/ConfigGroup.h"
#include "spjalla/lines/ConfigKey.h"
#include "spjalla/lines/Line.h"
#include "spjalla/lines/Warning.h"

#include "lib/formicine/futil.h"

#include "haunted/tests/Test.h"

namespace Spjalla {
	void Client::addCommands() {
		using sptr = PingPong::Server *;
		using line = const InputLine &;

		PingPong::Command::beforeSend = [&](PingPong::Command &cmd) { return beforeSend(cmd); };


		// Handles a fake line of input as if the client had read it from a socket.
		add("_fake", 0, -1, true, [&](sptr server, line il) {
			server->handleLine(PingPong::Line(server, il.body));
		});

		add("_front", 1, 1, true, [&](sptr server, line il) {
			std::shared_ptr<PingPong::Channel> channel = ui.activeWindow->channel;
			if (!channel) {
				ui.error("/_front needs a channel.");
				return;
			}

			std::shared_ptr<PingPong::User> user = server->getUser(il.first(), false, false);
			if (!user) {
				ui.error("Unknown user: " + il.first());
				return;
			}

			if (channel->sendToFront(user))
				ui.log("User " + ansi::bold(il.first()) + " sent to front of " + ansi::bold(channel->name) + ".");
			else
				ui.log("User " + ansi::bold(il.first()) + " wasn't sent to front of " + ansi::bold(channel->name)
					+ ".");

			std::string order = "Order of " + channel->name + ":";
			for (std::shared_ptr<PingPong::User> uptr: channel->users)
				order += " " + uptr->name;
			DBG(order);
		});

		add("_order", 0, 0, false, [&](sptr, line) {
			std::shared_ptr<PingPong::Channel> channel = ui.activeWindow->channel;
			if (!channel) {
				ui.error("/_order needs a channel.");
				return;
			}

			std::string order = "Order of " + channel->name + ":";
			for (std::shared_ptr<PingPong::User> user: channel->users)
				order += " " + user->name;
			DBG(order);
		});

		add("_recalc", 0, 0, false, [&](sptr, line) {
			for (const std::shared_ptr<Haunted::UI::TextLine> &textline: ui.activeWindow->getLines())
				textline->markDirty();
			ui.activeWindow->rowsDirty();
			ui.activeWindow->draw();
		});

		add("_lines", 0, 0, false, [&](sptr, line) {
			for (std::shared_ptr<Haunted::UI::TextLine> textline: ui.activeWindow->getLines()) {
				DBG("Line: " << std::string(*textline));
				DBG("   numRows_ = " << textline->numRows_);
				DBG("   #lines_  = " << textline->lines_.size());
			}

			DBG("totalRows_ = " << ui.activeWindow->totalRows_);
		});

		add("alias", 0, -1, false, [&](sptr, line il) { Commands::doAlias(*this, il); });

		add("ban", 1, 2, true, [&](sptr server, line il) {
			ban(server, il, "+b");
		});

		add("clear", 0, 0, false, [&](sptr, line) {
			if (UI::Window *window = ui.getActiveWindow())
				window->setVoffset(window->totalRows() - ui.scrollBuffer);
			else
				ui.error("No window.");
		});

		add("connect",    1,  2, false, [&](sptr,        line il) { Commands::doConnect(*this, il);            });
		add("disconnect", 0, -1, true,  [&](sptr server, line il) { Commands::doDisconnect(*this, server, il); });
		add("join",       1,  1, true,  [&](sptr server, line il) { Commands::doJoin(*this, server, il);       });

		add("kick", 1, -1, true, [&](sptr server, line il) {
			if (tripleCommand<PingPong::KickCommand>(server, il, ui.getActiveChannel()))
				noChannel();
		});

		add("me",   1, -1, true,  [&](sptr,      line il) {
			Commands::doMe(ui, il);
		}, Completions::completePlain);

		add("mode", 1, -1, true,  [&](sptr server, line il) { Commands::doMode(ui, server, il); });
		add("move", 1,  1, false, [&](sptr,        line il) { Commands::doMove(ui, il);         });

		add("msg", 2, -1, true, [&](sptr server, line il) {
			PingPong::PrivmsgCommand(server, il.first(), il.rest()).send();
		}, Completions::completePlain);

		add("nick",    0,  1, true,  [&](sptr server, line il) { Commands::doNick(ui, server, il);    });
		add("overlay", 0,  0, false, [&](sptr,        line)    { ui.updateOverlay();                });
		add("part",    0, -1, true,  [&](sptr server, line il) { Commands::doPart(*this, server, il); });
		add("plugin",  0, -1, false, [&](sptr,        line il) { Commands::doPlugin(*this, il);     });

		add("quit", 0, -1, false, [&](sptr, line il) {
			for (PingPong::Server *server: irc.serverOrder)
				server->quit(il.body);
		});

		add("quote", 1, -1, true, [&](sptr server, line il) { server->quote(il.body); });

		add("set",   0, -1, false, [&](sptr, line il) { Commands::doSet(*this, il); }, Completions::completeSet);
		add("spam",  0,  1, false, [&](sptr, line il) { Commands::doSpam(ui, il); });
		add("topic", 0, -1, true,  [&](sptr server, line il) { Commands::doTopic(*this, server, il); });

		add("unban", 1, 2, true, [&](sptr server, line il) {
			ban(server, il, "-b");
		});

		add("wc", 0, 0, false, [&](sptr, line) {
			if (ui.canRemove(ui.activeWindow))
				ui.removeWindow(ui.activeWindow);
		});

		add("whois", 1, 1, true, [&](sptr server, line il) {
			PingPong::WhoisCommand(server, il.first()).send();
		}, Completions::completePlain);
	}

	void Client::ban(PingPong::Server *server, const InputLine &il, const std::string &type) {
		std::shared_ptr<PingPong::Channel> chan = ui.getActiveChannel();
		std::string target;

		if (il.args.size() == 2) {
			if (il.args[0].front() != '#') {
				ui.warn("Invalid channel name: " + il.args[0]);
				return;
			}

			if (!server->getChannel(il.args[0], false)) {
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

		PingPong::ModeCommand(chan, type, target).send();
	}

	void Client::debugServers() {
		if (irc.servers.empty()) {
			DBG("No servers.");
			return;
		}

		for (const auto &pair: irc.servers) {
			PingPong::Server *server = pair.second;
			DBG(ansi::bold(server->id) << " (" << server->getNick() << "@"_d << server->hostname << "): "
				<< static_cast<int>(server->status));
			for (std::shared_ptr<PingPong::Channel> channel: server->channels) {
				DBG("    " << ansi::wrap(channel->name, ansi::style::underline) << " [" << channel->modeString()
				    << "]");
				for (std::shared_ptr<PingPong::User> user: channel->users) {
					std::string channels = "";
					std::vector<std::weak_ptr<PingPong::Channel>> expired_channels {};
					for (std::weak_ptr<PingPong::Channel> user_channel: user->channels) {
						if (user_channel.expired()) {
							expired_channels.push_back(user_channel);
						} else {
							channels += " " + user_channel.lock()->name;
						}
					}

					for (const std::weak_ptr<PingPong::Channel> &expired_channel: expired_channels)
						user->channels.erase(expired_channel);

					std::string to_dbg = "        " + user->name;
					if (!channels.empty())
						to_dbg += ":" + channels;

					if (user->idleSince != -1)
						to_dbg += "; idle since " + std::to_string(user->idleSince);

					if (user->signonTime != -1)
						to_dbg += "; signon time: " + std::to_string(user->signonTime);

					if (!user->realname.empty())
						to_dbg += "; realname: " + user->realname;

					DBG(to_dbg << "; ptr = " << user.get());
				}
			}
		}
	}
}
