#include <functional>
#include <ostream>
#include <string>

#include "ui/textinput.h"

namespace spjalla {
	void textinput::update() {
		if (on_update)
			on_update(buffer, cursor);
	}

	void textinput::listen(const std::function<void(const std::string &, int)> &fn) {
		on_update = fn;
	}

	void textinput::insert(const std::string &str) {
		buffer.insert(cursor, str);
		cursor += str.size();
		update();
	}

	void textinput::insert(char ch) {
		buffer.insert(cursor++, 1, ch);
		update();
	}

	void textinput::clear() {
		buffer.clear();
		cursor = 0;
		update();
	}

	void textinput::erase_word() {
		if (cursor == 0)
			return;
		size_t to_erase = 0;
		for (; get_char() == ' '; --cursor)
			to_erase++;
		for (; get_char() != '\0' && get_char() != ' '; --cursor)
			to_erase++;
		buffer.erase(cursor, to_erase);
		update();
	}

	void textinput::erase() {
		if (cursor > 0) {
			buffer.erase(--cursor, 1);
			update();
		}
	}

	void textinput::left() {
		if (cursor > 0) {
			--cursor;
			update();
		}
	}

	void textinput::right() {
		if (cursor != buffer.size()) {
			++cursor;
			update();
		}
	}

	size_t textinput::length() const {
		return buffer.length();
	}

	char textinput::get_char() const {
		return length() > 0 && cursor > 0? buffer[cursor - 1] : '\0';
	}

	std::string textinput::dbg_render() const {
		return "[" + buffer.substr(0, cursor) + "\e[2m|\e[0m" + buffer.substr(cursor) + "]";
	}

	textinput::operator std::string() const {
		return buffer;
	}

	std::ostream & operator<<(std::ostream &os, const textinput &input) {
		os << std::string(input);
		return os;
	}
}
