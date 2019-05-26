#include <iostream>
#include <memory>
#include <thread>

#include <csignal>
#include <ncurses.h>

#include "ui/ui.h"
#include "ui/point.h"

namespace spjalla {
	ui::~ui() {
		endwin();
	}

	void ui::draw() {
	}

	void ui::start() {
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, true);
		signal(SIGWINCH, &ui::handle_winch);
		worker_draw = std::make_shared<std::thread>(&ui::work_draw, this);
		worker_input = std::make_shared<std::thread>(&ui::work_input, this);
	}

	void ui::work_draw() {
		draw();
	}

	void ui::handle_winch(int) {
		endwin();
		refresh();
	}

	void ui::work_input() {
		for (;;) {
			int c = getch();

			// if (c != -1) std::cout << c << "/" << static_cast<char>(c) << "\r" << std::endl; continue;

			switch (c) {
				case -1:
					break;
				case 22: // ^U
					input_buffer.clear();
					cursor = 0;
					render_input();
					break;
				case 258: break; // down
				case 259: break; // up
				case 260: // left
					if (cursor > 0) cursor--;
					break;
				case 261: // right
					if (cursor != input_buffer.size()) cursor++;
					break;
				case 'k': {
					int x = 666, y = 666;
					getmaxyx(stdscr, x, y);
					std::cout << "(" << x << ", " << y << ")";
					std::cout << "; (" << COLS << ", " << LINES << ")";
					// std::cout << "; (" << SCR_W << ", " << SCR_H << ")";
					std::cout << "\r" << std::endl;
					break;
				}
				case '\r':
				case '\n':
					process_input();
					break;
				default:
					input_buffer.insert(cursor++, 1, static_cast<char>(c));
			}
		}
	}

	void ui::render_input() {

	}

	void ui::process_input() {
		std::string input = input_buffer;
		input_buffer = "";
		cursor = 0;
		// input_buffer >> input;
		// input_buffer.str("");
		std::cout << "[" << input << "]\r" << std::endl;
	}

	void ui::join() {
		worker_draw->join();
		worker_input->join();
		endwin();
	}
}
