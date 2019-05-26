#ifndef UI_UI_H_
#define UI_UI_H_

#include <memory>
#include <sstream>
#include <thread>

namespace spjalla {
	class ui {
		private:
			// std::stringstream input_buffer;
			std::string input_buffer;
			size_t cursor = 0;
			std::shared_ptr<std::thread> worker_draw, worker_input;
			void work_draw();
			void work_input();
			void process_input();
			void render_input();
			static void handle_winch(int);

		public:
			ui() {}
			~ui();

			void draw();
			void start();
			void join();
	};
}

#endif
