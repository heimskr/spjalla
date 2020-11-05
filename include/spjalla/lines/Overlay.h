#ifndef SPJALLA_LINES_OVERLAY_H_
#define SPJALLA_LINES_OVERLAY_H_

#include "pingpong/core/Server.h"
#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	/**
	 * Represents a line in the overlay for the status window that represents a server.
	 */
	struct StatusServerLine: public Line {
		PingPong::Server *server;

		StatusServerLine(Client *parent_, PingPong::Server *server_, long stamp_ = PingPong::Util::timestamp()):
			Line(parent_, stamp_, 2), server(server_) {}

		virtual std::string render(UI::Window *) override;
	};

	/**
	 * Represents a line in the overlay for the status window that represents a channel under a server.
	 */
	struct StatusChannelLine: public Line {
		std::shared_ptr<PingPong::Channel> channel;

		StatusChannelLine(Client *parent_, std::shared_ptr<PingPong::Channel> channel_, long stamp_ =
		PingPong::Util::timestamp()):
			Line(parent_, stamp_, 4), channel(channel_) {}

		virtual std::string render(UI::Window *) override;
	};
}

#endif
