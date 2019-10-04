#ifndef PINGPONG_CORE_INPUT_LINE_H_
#define PINGPONG_CORE_INPUT_LINE_H_

#include <string>
#include <vector>

#include "lib/formicine/futil.h"

namespace spjalla {
	class input_line {
		private:
			bool minimal = false; // Whether the command is "/".

		public:
			std::string command, body;
			std::vector<std::string> args;
			std::string original;

			input_line() = delete;
			input_line(const std::string &command_, const std::string &body_):
				command(formicine::util::lower(command_)), body(body_), original("/" + command_ + " " + body_) {}
			input_line(const std::string &full);

			inline bool is_command() const { return minimal || !command.empty(); }
			std::string first() const;
			std::string rest() const;
			operator std::string() const;

			friend std::ostream & operator<<(std::ostream &, const input_line &);
	};
}

#endif
