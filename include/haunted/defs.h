#ifndef HAUNTED_DEFS_H_
#define HAUNTED_DEFS_H_

namespace haunted {
	enum side {left, right, top, bottom};

	struct position {
		int left, top, width, height;
		position(int l, int t, int w, int h): left(l), top(t), width(w), height(h) {}
		position(): left(0), top(0), width(0), height(0) {}
	};
}

#endif
