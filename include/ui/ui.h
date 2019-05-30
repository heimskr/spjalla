#ifndef UI_UI_H_
#define UI_UI_H_

#include <deque>
#include <memory>
#include <sstream>
#include <thread>

#include "haunted/ui/textinput.h"

namespace spjalla {
	class ui {
		private:
			haunted::ui::textinput input;
			std::shared_ptr<std::thread> worker_draw, worker_input;

			void work_draw();
			void work_input();
			void process_input();
			void render_input();
			haunted::position get_chat_position();
			haunted::position get_users_position();
			haunted::position get_input_position();
			static void handle_winch(int);

		public:
			haunted::side users_side = haunted::side::left;
			double users_width = 0.2;
			size_t max_lines = 128;

			ui() {}

			void draw();
			void start();
			void join();
	};
}

#endif
