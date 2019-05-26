#ifndef UI_UI_H_
#define UI_UI_H_

#include <deque>
#include <memory>
#include <sstream>
#include <thread>

#include <ncurses.h>

#include "ui/textinput.h"

namespace spjalla {
	class ui {
		private:
			textinput input;
			std::shared_ptr<std::thread> worker_draw, worker_input;
			WINDOW *chat_window, *users_window, *input_window;
			bool alt = false;

			void work_draw();
			void work_input();
			void process_input();
			void render_input();
			rect get_chat_rect();
			rect get_users_rect();
			rect get_input_rect();
			static void handle_winch(int);

		public:
			side users_side = left;
			double users_width = 0.2;
			size_t max_lines = 128;

			ui();
			~ui();

			void draw();
			void start();
			void join();
	};
}

#endif
