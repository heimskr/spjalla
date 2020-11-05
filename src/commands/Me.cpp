#include "pingpong/commands/Privmsg.h"
#include "spjalla/commands/Command.h"
#include "spjalla/ui/Interface.h"

namespace Spjalla::Commands {
	void doMe(UI::Interface &ui, const InputLine &il) {
		const UI::Window *window = ui.getActiveWindow();
		if (window->isDead())
			return;

		const std::string message = "\1ACTION " + il.body + "\1";
		if (window->isChannel())
			PingPong::PrivmsgCommand(window->channel, message).send();
		else if (window->isUser())
			PingPong::PrivmsgCommand(window->user, message).send();
	}
}
