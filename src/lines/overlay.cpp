#include "spjalla/lines/Overlay.h"

namespace Spjalla::Lines {
	std::string StatusServerLine::render(UI::Window *) {
		std::shared_ptr<PingPong::User> self = server->getSelf();
		std::string out = "- "_d + ansi::wrap(server->id, ansi::style::underline) + " (" + server->getNick() + "@"_d +
			server->hostname + ")";
		if (!self->modes.empty())
			out += ": "_d + self->modeString();
		return out;
	}

	std::string StatusChannelLine::render(UI::Window *) {
		std::string out = "  - "_d + channel->name;
		if (!channel->modes.empty())
			out += ": "_d + channel->modeString();
		return out;
	}
}
