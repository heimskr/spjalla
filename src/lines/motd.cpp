#include "pingpong/core/Util.h"
#include "spjalla/lines/Motd.h"

namespace Spjalla::Lines {
	MotdLine::MotdLine(Client *parent_, const std::string &text_, PingPong::Server *server_, long stamp_):
		Line(parent_, stamp_, 0), server(server_), text(text_) {}

	PingPong::Server * MotdLine::getAssociatedServer() const {
		return server;
	}

	std::string MotdLine::render(UI::Window *) {
		return PingPong::Util::irc2ansi(text);
	}
}
