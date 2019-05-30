#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include <csignal>

#include "ui/ui.h"
#include "ui/defs.h"
#include "ui/boxes/hbox_propo.h"

namespace spjalla {
	ui::ui() {
		rect chat_rect  = get_chat_rect(),
		     users_rect = get_users_rect(),
		     input_rect = get_input_rect();
		chat_window  = newwin(chat_rect.h,  chat_rect.w,  chat_rect.y,  chat_rect.y);
		users_window = newwin(users_rect.h, users_rect.w, users_rect.y, users_rect.y);
		input_window = newwin(input_rect.h, input_rect.w, input_rect.y, input_rect.y);
		box(chat_window,  0, 0);
		box(users_window, 0, 0);
		box(input_window, 0, 0);
	}

	void ui::draw() {
	}

	void ui::start() {
		// initscr();
		// start_color();
		// cbreak();
		// noecho();
		// keypad(stdscr, true);
		signal(SIGWINCH, &ui::handle_winch);
		worker_draw  = std::make_shared<std::thread>(&ui::work_draw,  this);
		worker_input = std::make_shared<std::thread>(&ui::work_input, this);
	}

	void ui::work_draw() {
		draw();
	}

	void ui::handle_winch(int) {
		
	}

	void ui::work_input() {
		using std::cout, std::endl;
		return;
		for (;;) {
			int c = getch();

			// if (c != -1) std::cout << c << "/" << static_cast<char>(c) << "\r" << std::endl; //continue;

			if (c == 27) { // ^[ (alt)
				alt = true;
				continue;
			}

			if (alt) {
				switch (c) {
					case 'b': // alt+left
						input.prev_word();
						break;
					case 'f': // alt+right
						input.next_word();
						break;
					case 127:
					case KEY_BACKSPACE:
						input.erase_word();
						break;
				}
			} else {
				switch (c) {
					case -1:
					case 14:  // ^N
					case 16:  // ^P
					case 258: // down
					case 259: // up
					case KEY_RESIZE:
						break;
					case 21: // ^U
						input.clear();
						break;
					case 23: // ^W
						input.erase_word();
						break;
					case 127: // backspace
					case KEY_BACKSPACE:
						input.erase();
						break;
					case KEY_F(1):
						std::cout << "[" << input << "]\r" << std::endl;
						break;
					case 260: // left
						input.left();
						break;
					case 261: // right
						input.right();
						break;
					case '\r':
					case '\n':
						process_input();
						break;
					default:
						if (c <= 0xff)
							input.insert(static_cast<char>(c));
				}
			}

			std::string str(1, c);

			cout << "\e[2K\e[G" << input.dbg_render(false) << "\r\n\e[2K" << std::setfill(' ') << std::setw(2)
			     << std::left << input.length() << " " << std::setw(3) << std::right << c << (alt? " Alt+" : " ")
			     << keyname(c) << "\e[A\e[" << 2 + input.get_cursor() << "G";
			cout.flush();
			alt = false;
		}
	}

	void ui::render_input() {

	}

	void ui::process_input() {
		std::cout << "\r\e[2KString: \"" << input << "\" [" << input.length() << "]\r\n\e[2K\e[2G";
		input.clear();
	}

	void ui::join() {
		worker_draw->join();
		worker_input->join();
		endwin();
	}

	rect ui::get_chat_rect() {
		int uwidth = static_cast<int>(COLS * users_width);
		return {users_side == right? 0 : uwidth, 0, COLS - uwidth, LINES - get_input_rect().h};
	}

	rect ui::get_users_rect() {
		int uwidth = static_cast<int>(COLS * users_width);
		return {users_side == left? 0 : COLS - uwidth, 0, uwidth, LINES - get_input_rect().h};
	}

	rect ui::get_input_rect() {
		return {0, LINES - 1, COLS, 1};
	}
}
