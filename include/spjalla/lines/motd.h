#ifndef SPJALLA_LINES_MOTD_H_
#define SPJALLA_LINES_MOTD_H_

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	class MotdLine: public Line {
		private:
			PingPong::Server *server;

		protected:
			virtual PingPong::Server * getAssociatedServer() const override;

		public:
			std::string text;

			MotdLine(Client *, const std::string &, PingPong::Server *, long stamp_ = now());

			virtual std::string render(UI::Window *) override;
	};
}

#endif
