#ifndef HAUNTED_CONTROL_H_
#define HAUNTED_CONTROL_H_

#include <vector>

namespace haunted {
	/**
	 * Represents a control.
	 * This includes things like boxes, text views and text inputs.
	 */
	class control {
		protected:
			std::vector<control *> children;
			position pos;
			control(const position &pos_): pos(pos_) {}

		public:
			virtual ~control() = 0;
			virtual ssize_t max_children() const;
			virtual size_t   child_count() const;
	};
}

#endif
