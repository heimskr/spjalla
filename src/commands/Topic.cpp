#include "pingpong/core/Channel.h"

#include "spjalla/commands/Command.h"
#include "spjalla/core/Client.h"
#include "spjalla/lines/Topic.h"

namespace Spjalla::Commands {
	void doTopic(Client &cli, PingPong::Server *server, const InputLine &il) {
		UI::Interface &ui = cli.getUI();
		if (il.args.size() < 2) {
			std::shared_ptr<PingPong::Channel> chan = il.args.empty()?
				ui.getActiveChannel() : server->getChannel(il.first(), false);
			if (!chan) {
				ui.warn("No channel selected.");
				return;
			}

			UI::Window *win = ui.getWindow(chan, false);
			if (!win)
				win = ui.getWindow();

			ui.log(Lines::TopicLine(&cli, "", chan->name, chan->topic, PingPong::Util::timestamp()), win);
		}
	}
}
