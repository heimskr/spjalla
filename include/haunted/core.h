#ifndef HAUNTED_HAUNTED_H_
#define HAUNTED_HAUNTED_H_

#include <memory>
#include <termios.h>

namespace haunted {
	class haunted {
		private:
			static std::unique_ptr<termios> original;

		public:
			static termios getattr();
			static void setattr(const termios &);
			static void resetattr();
			static void cbreak();
			static void cleanup();
	};
}

#endif
