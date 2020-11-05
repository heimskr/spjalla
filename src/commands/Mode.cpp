#include "pingpong/commands/Mode.h"
#include "spjalla/commands/Command.h"
#include "spjalla/ui/Interface.h"

namespace Spjalla::Commands {
	void doMode(UI::Interface &ui, PingPong::Server *server, const InputLine &il) {

		std::shared_ptr<PingPong::Channel> win_chan = ui.getActiveChannel();

		if (il.args.size() == 1 && il.first().find_first_of("+-") == 0) {
			// The only argument is a set of flags. If the active window is a channel, this sets the flags on the
			// channel. Otherwise, the flags are set on yourself.
			if (win_chan)
				PingPong::ModeCommand(win_chan, il.first()).send();
			else
				PingPong::ModeCommand(server->getNick(), server, il.first()).send();
			return;
		}

		std::string chan_str {}, flags {}, extra {};

		// Look through all the arguments.
		for (const std::string &arg: il.args) {
			char front = arg.front();
			if (front == '#') {
				if (!chan_str.empty()) {
					ui.warn("You cannot set modes for multiple channels in one /mode command.");
					return;
				}

				chan_str = arg;
			} else if (front == '+' || front == '-') {
				if (arg.find_first_not_of(PingPong::Util::flagChars) != std::string::npos) {
					ui.warn("Invalid flags for mode command: " + arg);
					return;
				}

				if (flags.empty()) {
					flags = arg;
				} else {
					ui.warn("You cannot set multiple sets of flags in one /mode command.");
					return;
				}
			} else if (extra.empty()) {
				extra = arg;
			} else {
				// No overwriting the extra parameters.
				ui.warn("You cannot set flags for multiple targets in one /mode command.");
				return;
			}
		}

		// You can't set modes without flags.
		if (flags.empty()) {
			ui.warn("No flags specified for /mode.");
			return;
		}

		// If there's no channel indicated either in the command arguments or by the active window and you're not
		// trying to set user flags on yourself, then what are you even trying to do? You can't set flags on someone
		// who isn't you.
		if (!win_chan && chan_str.empty() && extra != server->getNick()) {
			ui.warn("Invalid arguments for /mode.");
			return;
		}

		// If there's no channel and we're setting arguments on ourself, it's a regular user mode command.
		if (!win_chan && chan_str.empty() && extra == server->getNick()) {
			PingPong::ModeCommand(server->getSelf(), flags).send();
			return;
		}

		if (chan_str.empty() && win_chan)
			chan_str = win_chan->name;

		// At this point, I think it's safe to assume that you're setting channel flags. The extra parameter, if
		// present, is what/whom you're setting the flags on.
		PingPong::ModeCommand(chan_str, server, flags, extra).send();

	}
}
