#ifndef SPJALLA_LINES_OVERLAY_H_
#define SPJALLA_LINES_OVERLAY_H_

#include "pingpong/core/server.h"
#include "spjalla/lines/line.h"

namespace spjalla::lines {
	/**
	 * Represents a line in the overlay for the status window that represents a server.
	 */
	struct status_server_line: public line {
		pingpong::server *serv;

		status_server_line(client *parent_, pingpong::server *serv_, long stamp_ = pingpong::util::timestamp()):
			line(parent_, stamp_, 2), serv(serv_) {}

		virtual std::string render(ui::window *) override;
		virtual operator std::string() override;
	};

	/**
	 * Represents a line in the overlay for the status window that represents a channel under a server.
	 */
	struct status_channel_line: public line {
		std::shared_ptr<pingpong::channel> chan;

		status_channel_line(client *parent_, std::shared_ptr<pingpong::channel> chan_, long stamp_ =
		pingpong::util::timestamp()):
			line(parent_, stamp_, 4), chan(chan_) {}

		virtual std::string render(ui::window *) override;
		virtual operator std::string() override;
	};
}

#endif
