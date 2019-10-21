#include "spjalla/lines/overlay.h"

namespace spjalla::lines {
	std::string status_server_line::render(ui::window *) {
		std::shared_ptr<pingpong::user> self = serv->get_self();
		std::string out = "- "_d + ansi::wrap(serv->id, ansi::style::underline) + " (" + serv->get_nick() + "@"_d +
			serv->hostname + ")";
		if (!self->modes.empty())
			out += ": "_d + self->mode_str();
		return out;
	}

	std::string status_channel_line::render(ui::window *) {
		std::string out = "  - "_d + chan->name;
		if (!chan->modes.empty())
			out += ": "_d + chan->mode_str();
		return out;
	}

	status_server_line::operator std::string() {
		return render(nullptr);
	}

	status_channel_line::operator std::string() {
		return render(nullptr);
	}
}
