#ifndef UI_TEXTINPUT_H_
#define UI_TEXTINPUT_H_

#include <cstddef>
#include <functional>
#include <string>

namespace spjalla {
	class textinput {
		using update_fn = std::function<void(const std::string &, int)>;

		private:
			std::string buffer;
			size_t cursor;
			update_fn on_update;
			void update();

		public:
			textinput(std::string buffer_, size_t cursor_): buffer(buffer_), cursor(cursor_) {}
			textinput(std::string buffer_): textinput(buffer_, 0) {}
			textinput(size_t cursor_): textinput("", cursor_) {}
			textinput(): textinput("", 0) {}

			operator std::string() const;

			void listen(const update_fn &);
			void move_to(size_t);
			void insert(const std::string &);
			void insert(char);
			void clear();
			void erase_word();
			void erase();
			std::string get_text() const;
			void set_text(const std::string &);
			void left();
			void right();
			void start();
			void end();
			size_t size() const;
			char get_char() const;

			std::string dbg_render() const;

			friend std::ostream & operator<<(std::ostream &os, const textinput &input);
	};
}

#endif
