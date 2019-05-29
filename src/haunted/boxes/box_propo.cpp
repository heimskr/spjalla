#include <stdexcept>

#include "haunted/boxes/box_propo.h"

namespace haunted::boxes {
	box_propo::box_propo(const position &pos_, double ratio_): control(pos_), ratio(ratio_) {
		if (ratio_ < 0)
			throw std::domain_error("Box ratio cannot be negative");
	}

	int box_propo::size_one() const {
		return size - size_two();
	}

	int box_propo::size_two() const {
		return size / (1.0 + ratio);
	}
	
}
