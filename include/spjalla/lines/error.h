#ifndef SPJALLA_LINES_ERROR_H_
#define SPJALLA_LINES_ERROR_H_

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	class ErrorLine: public Line {
		protected:
			virtual PingPong::Server * getAssociatedServer() const override { return server; }

		public:
			std::string message;
			PingPong::Server *server = nullptr;

			ErrorLine(Client *parent_, const std::string &message_, long stamp_ = PingPong::Util::timestamp()):
				Line(parent_, stamp_, ansi::length(Lines::redNotice)), message(message_) {}

			virtual std::string render(UI::Window *) override;
			virtual NotificationType getNotificationType() const override { return NotificationType::Highlight; }
	};
}

#endif
