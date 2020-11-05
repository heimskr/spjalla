#ifndef SPJALLA_LINES_RAW_H_
#define SPJALLA_LINES_RAW_H_

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	class RawLine: public Line {
		protected:
			virtual PingPong::Server * getAssociatedServer() const override { return server; }

		public:
			std::string text;
			PingPong::Server *server;
			bool isBad = false;
			/** True if the line is for content that was sent to the server, false if from the server. */
			bool isOut = false;

			RawLine(Client *parent_, const std::string &text_, PingPong::Server *server_, bool is_out = false,
			bool is_bad = false, long stamp_ = now()):
				Line(parent_, stamp_, 3), text(text_), server(server_), isBad(is_bad), isOut(is_out) {}

			virtual std::string render(UI::Window *) override;
	};
}

#endif
