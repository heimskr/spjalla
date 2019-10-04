#ifndef SPJALLA_LINES_OVERLAY_H_
#define SPJALLA_LINES_OVERLAY_H_

#include "pingpong/core/server.h"
#include "spjalla/lines/line.h"

namespace spjalla::lines {
	/**
	 * Represents a line in the overlay for the status window that represents a server.
	 */
	struct status_server_line: haunted::ui::textline {
		pingpong::server *serv;

		status_server_line(pingpong::server *serv_): haunted::ui::textline(2), serv(serv_) {}

		virtual operator std::string() const override;
	};

	/**
	 * Represents a line in the overlay for the status window that represents a channel under a server.
	 */
	struct status_channel_line: haunted::ui::textline {
		std::shared_ptr<pingpong::channel> chan;

		status_channel_line(std::shared_ptr<pingpong::channel> chan_): haunted::ui::textline(4), chan(chan_) {}

		virtual operator std::string() const override;
	};
}

#endif
