#ifndef SPJALLA_LINES_SERVERBASIC_H_
#define SPJALLA_LINES_SERVERBASIC_H_

#include "pingpong/core/Util.h"

#include "spjalla/lines/Basic.h"

namespace Spjalla::Lines {
	class ServerBasicLine: public BasicLine {
		private:
			PingPong::Server *server;

		public:
			ServerBasicLine(Client *parent_, PingPong::Server *server_, const std::string &text_,
			int base_continuation = 0, long stamp_ = PingPong::Util::timestamp()):
				BasicLine(parent_, text_, base_continuation, stamp_), server(server_) {}

			ServerBasicLine(PingPong::Server *server_, const std::string &text_, int base_continuation = 0,
			long stamp_ = PingPong::Util::timestamp()):
				ServerBasicLine(nullptr, server_, text_, base_continuation, stamp_) {}

			virtual std::string render(UI::Window *) override { return text; }
			virtual PingPong::Server * getAssociatedServer() const override { return server; }
	};
}

#endif
