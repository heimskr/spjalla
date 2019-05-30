#ifndef HAUNTED_HAUNTED_H_
#define HAUNTED_HAUNTED_H_

#include <memory>
#include <termios.h>

namespace haunted {
	class core {
		private:
			static std::unique_ptr<termios> original, copy;
			static termios & attrs();
			static termios getattr();
			static void setattr(const termios &);
			static void resetattr();

		public:
			static void cbreak();
			static void cleanup();
	};
}

#endif
