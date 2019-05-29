#include "haunted/colors.h"

namespace spjalla {
	chtype colors::next_index = 1;
	std::unordered_map<colors::color_num, chtype> colors::index2color {};
	std::unordered_map<chtype, colors::color_num> colors::color2index {};

	chtype colors::get_index(long r, long g, long b) {
		color_num color = pack(r, g, b);
		auto found = color2index.find(color);
		if (found != color2index.end())
			return found->second;
		return new_color(color, r, g, b);
	}

	chtype colors::new_color(color_num packed, long r, long g, long b) {
		chtype index = next_index++;
		init_color(index, r, g, b);
		index2color.insert({packed, index});
		color2index.insert({index, packed});
		return index;
	}

	colors::color_num colors::pack(long r, long g, long b) {
		return r | (g << 16L) | (b << 32L);
	}

	colors::color_triplet colors::unpack(color_num color) {
		short r, g, b;
		unpack(color, r, g, b);
		return {r, g, b};
	}

	void colors::unpack(color_num color, short &r, short &g, short &b) {
		r = color & 0xffff;
		g = (color >> 16L) & 0xffff;
		b = (color >> 32L) & 0xffff;
	}

	void colors::setfg(WINDOW *win, short r, short g, short b) {
		wbkgd(win, get_index(r, g, b));
	}
}
