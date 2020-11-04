#ifndef PINGPONG_CORE_INPUTLINE_H_
#define PINGPONG_CORE_INPUTLINE_H_

#include <string>
#include <vector>

#include "lib/formicine/futil.h"

namespace Spjalla {
	class InputLine {
		private:
			bool minimal = false; // Whether the command is "/".

		public:
			std::string command, body;
			std::vector<std::string> args;
			std::string original;

			InputLine() = delete;
			InputLine(const std::string &command_, const std::string &body_):
				command(formicine::util::lower(command_)), body(body_), original("/" + command_ + " " + body_) {}
			InputLine(const std::string &full);

			inline bool isCommand() const { return minimal || !command.empty(); }
			std::string first() const;
			std::string rest() const;
			operator std::string() const;

			friend std::ostream & operator<<(std::ostream &, const InputLine &);
	};
}

#endif
