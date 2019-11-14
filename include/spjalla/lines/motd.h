#ifndef SPJALLA_LINES_MOTD_H_
#define SPJALLA_LINES_MOTD_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class motd_line: public line {
		private:
			pingpong::server *serv;

		protected:
			virtual pingpong::server * get_associated_server() const override;

		public:
			std::string text;

			motd_line(client *parent_, const std::string &text_, pingpong::server *serv_, long stamp_ = now());

			virtual std::string render(ui::window *) override;
	};
}

#endif
