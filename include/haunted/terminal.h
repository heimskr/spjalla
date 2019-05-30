#ifndef HAUNTED_TERMINAL_H_
#define HAUNTED_TERMINAL_H_

#include <memory>
#include <termios.h>

namespace haunted {
	class terminal {
		private:
			termios & attrs();
			termios getattr();
			void setattr(const termios &);
			void resetattr();

		public:
			termios *original, *current;

			terminal(): original(nullptr), current(nullptr) {}
			~terminal();

			void cbreak();
	};
}

#endif
