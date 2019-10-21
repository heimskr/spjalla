#ifndef SPJALLA_LINES_RAW_H_
#define SPJALLA_LINES_RAW_H_

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class raw_line: public line {
		protected:
			virtual pingpong::server * get_associated_server() const override { return serv; }

		public:
			std::string text;
			pingpong::server *serv;
			bool is_bad = false;
			/** True if the line is for content that was sent to the server, false if from the server. */
			bool is_out = false;

			raw_line(client *parent_, const std::string &text_, pingpong::server *serv_, bool is_out_ = false,
			bool is_bad_ = false, long stamp_ = now()):
				line(parent_, stamp_, 3), text(text_), serv(serv_), is_bad(is_bad_), is_out(is_out_) {}

			virtual std::string render(ui::window *) override;
	};
}

#endif
