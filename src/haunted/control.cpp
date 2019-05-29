#include "haunted/control.h"

namespace haunted {
	int control::max_children() const {
		return -1;
	}

	int control::child_count() const {
		return children.size();
	}
}
