#ifndef HAUNTED_COLORS_H_
#define HAUNTED_COLORS_H_

#include <tuple>
#include <unordered_map>
#include <utility>

#include <ncurses.h>

namespace spjalla {
	class colors {
		private:
			using color_num = long;
			using color_triplet = std::tuple<short, short, short>;

			static chtype next_index;
			static std::unordered_map<color_num, chtype> index2color;
			static std::unordered_map<chtype, color_num> color2index;

			static chtype get_index(long r, long g, long b);
			static chtype new_color(color_num, long r, long g, long b);

			static color_num pack(long r, long g, long b);
			static color_triplet unpack(color_num);
			static void unpack(color_num, short &r, short &g, short &b);

		public:
			static void setfg(WINDOW *, short r, short g, short b);
	};
}

#endif
