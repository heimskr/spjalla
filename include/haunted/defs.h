#ifndef HAUNTED_DEFS_H_
#define HAUNTED_DEFS_H_

namespace haunted {
	enum side {left, right, top, bottom};

	struct position {
		size_t left, top, width, height;
		position(size_t l, size_t t, size_t w, size_t h): left(l), top(t), width(w), height(h) {}
	};
}

#endif
