#ifndef CORE_INPUT_LINE_H_
#define CORE_INPUT_LINE_H_

#include <string>
#include <vector>

namespace spjalla {
	class input_line {
		private:
			bool minimal = false; // Whether the command is "/".

		public:
			std::string command, body;
			std::vector<std::string> args;

			input_line(std::string command_, std::string body_): command(command_), body(body_) {}
			input_line(std::string full);

			inline bool is_command() const { return minimal || !command.empty(); }
			std::string first() const;
			std::string rest() const;
			operator std::string() const;
	};
}

#endif
