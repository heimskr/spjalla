#include "pingpong/core/util.h"
#include "spjalla/lines/motd.h"

namespace spjalla::lines {
	motd_line::motd_line(client *parent_, const std::string &text_, pingpong::server *serv_, long stamp_):
		line(parent_, stamp_, 0), serv(serv_), text(text_) {}

	pingpong::server * motd_line::get_associated_server() const {
		return serv;
	}

	std::string motd_line::render(ui::window *) {
		return pingpong::util::irc2ansi(text);
	}
}
