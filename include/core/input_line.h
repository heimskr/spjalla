#ifndef CORE_INPUT_LINE_H_
#define CORE_INPUT_LINE_H_

#include <string>

namespace spjalla {
	class input_line {
		private:
			std::string command;
			std::string body;
			bool minimal = false; // Whether the command is "/".

		public:
			input_line(std::string command_, std::string body_): command(command_), body(body_) {}
			input_line(std::string full);

			inline bool is_command() const { return minimal || !command.empty(); }
			operator std::string() const;
	};
}

#endif
