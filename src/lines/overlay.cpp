#include "spjalla/lines/overlay.h"

namespace spjalla::lines {
	status_server_line::operator std::string() const {
		std::shared_ptr<pingpong::user> self = serv->get_self();
		std::string out = "- "_d + std::string(*serv);
		if (!self->modes.empty())
			out += ": "_d + self->mode_str();
		return out;
	}

	status_channel_line::operator std::string() const {
		std::string out = "  - "_d + chan->name;
		if (!chan->modes.empty())
			out += ": "_d + chan->mode_str();
		return out;
	}
}
