#ifndef UI_UI_H_
#define UI_UI_H_

#include <memory>
#include <sstream>
#include <thread>

#include <ncurses.h>

namespace spjalla {
	struct rect {
		int x, y, w, h;
		rect(int x_, int y_, int w_, int h_): x(x_), y(y_), w(w_), h(h_) {}
	};

	class ui {
		private:
			std::string input_buffer;
			size_t cursor = 0;
			std::shared_ptr<std::thread> worker_draw, worker_input;
			WINDOW *chat_window;
			WINDOW *users_window;
			WINDOW *input_window;

			void work_draw();
			void work_input();
			void process_input();
			void render_input();
			rect get_chat_rect();
			rect get_users_rect();
			rect get_input_rect();
			static void handle_winch(int);

		public:
			enum side {left, right};
			side users_side = left;
			double users_width = 0.2;

			ui();
			~ui();

			void draw();
			void start();
			void join();
	};
}

#endif
