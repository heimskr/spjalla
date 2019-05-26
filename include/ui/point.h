#ifndef UI_POINT_H_
#define UI_POINT_H_

namespace spjalla {
	struct point {
		size_t x, y;
		point(size_t x_, size_t y_): x(x_), y(y_) {}
	};
}

#endif
