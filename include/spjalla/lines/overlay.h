#ifndef SPJALLA_LINES_OVERLAY_H_
#define SPJALLA_LINES_OVERLAY_H_

#include "pingpong/core/server.h"
#include "spjalla/lines/lines.h"

namespace spjalla::lines {
	/**
	 * Represents a line in the overlay for the status window that represents a server.
	 */
	struct status_server_line: line {
		pingpong::server *serv;

		status_server_line(pingpong::server *serv_): line(2), serv(serv_) {}

		virtual operator std::string() const override;
	};

	/**
	 * Represents a line in the overlay for the status window that represents a channel under a server.
	 */
	struct status_channel_line: line {
		std::shared_ptr<pingpong::channel> chan;

		status_channel_line(std::shared_ptr<pingpong::channel> chan_): line(4), chan(chan_) {}

		virtual operator std::string() const override;
	};
}

#endif
