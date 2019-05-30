#ifndef HAUNTED_TERMINAL_H_
#define HAUNTED_TERMINAL_H_

#include <memory>
#include <termios.h>

namespace haunted {
	/**
	 * This class enables interaction with terminals.
	 * It makes use of termios to change terminal modes.
	 * With its destructor, it resets 
	 */
	class terminal {
		private:
			termios original;
			termios getattr();
			void setattr(const termios &);
			void apply();
			void reset();

		public:
			termios attrs;

			terminal();
			~terminal();

			void cbreak();
	};
}

#endif
