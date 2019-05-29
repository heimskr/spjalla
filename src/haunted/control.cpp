#include "haunted/control.h"

namespace haunted {
	ssize_t control::max_children() const {
		return -1;
	}

	size_t control::child_count() const {
		return children.size();
	}
}
