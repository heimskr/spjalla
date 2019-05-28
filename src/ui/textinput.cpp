#include <functional>
#include <ostream>
#include <string>

#include "ui/textinput.h"
#include "unicode/uniset.h"

namespace spjalla {
	void textinput::update() {
		if (on_update)
			on_update(buffer, cursor);
	}

	void textinput::listen(const std::function<void(const std::string &, int)> &fn) {
		on_update = fn;
	}

	void textinput::move_to(size_t new_cursor) {
		if (new_cursor <= size() && new_cursor != cursor) {
			cursor = new_cursor;
			update();
		}
	}

	void textinput::insert(const std::string &str) {
		buffer.insert(cursor, str);
		cursor += str.size();
		update();
	}

	void textinput::insert(char ch) {
		if (!unicode_buffer.empty()) {
			unicode_buffer.push_back(ch);
			// if (utf8::is_valid(unicode_buffer.begin(), unicode_buffer.end())) {
			if (false) {
				// The unicode buffer now contains a complete and valid codepoint, so we insert it into the buffer.
				buffer.insert(cursor, unicode_buffer);
				unicode_buffer.clear();
				++cursor;
				update();
			} else if (unicode_buffer.size() == 4) {
				// At this point, it seems there's just garbage in the buffer. Insert it.
				buffer.insert(cursor, unicode_buffer);
				unicode_buffer.clear();
				cursor += 4;
				update();
			}
		} else {
			std::string str(1, ch);
			// if (!is_incomplete(ch)) {
			if (true) {
				buffer.insert(cursor++, 1, ch);
				update();
			} else {
				unicode_buffer.push_back(ch);
			}
		}
	}

	// bool textinput::is_incomplete(const std::string &str) {
	// 	unsigned int cp = 0;
	// 	for (size_t i = 0; i < str.size(); ++i)
	// 		cp |= str[i] << (i << 3);
	// 	return !utf8::internal::is_code_point_valid(cp);
	// }

	// bool textinput::is_incomplete(char ch) {
	// 	std::string str(1, ch);
	// 	auto it = str.begin();
	// 	return utf8::internal::validate_next(it, str.end()) != 4;
	// }

	void textinput::clear() {
		buffer.clear();
		cursor = 0;
		update();
	}

	void textinput::erase_word() {
		if (cursor == 0)
			return;
		size_t to_erase = 0;
		for (; prev_char() == ' '; --cursor)
			to_erase++;
		for (; prev_char() != '\0' && prev_char() != ' '; --cursor)
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

	std::string textinput::get_text() const {
		return buffer;
	}

	void textinput::set_text(const std::string &str) {
		buffer = str;
		cursor = str.size();
		update();
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

	void textinput::start() {
		if (cursor != 0) {
			cursor = 0;
			update();
		}
	}

	void textinput::end() {
		if (cursor != size()) {
			cursor = size();
			update();
		}
	}

	void textinput::prev_word() {
		if (cursor == 0)
			return;
		size_t old_cursor = cursor;
		for (; prev_char() == ' '; --cursor);
		for (; prev_char() != '\0' && prev_char() != ' '; --cursor);
		if (cursor != old_cursor)
			update();
	}

	void textinput::next_word() {
		// if (cursor == size())
		// 	return;
		size_t old_cursor = cursor;
		for (; next_char() == ' '; ++cursor);
		for (; next_char() != '\0' && next_char() != ' '; ++cursor);
		if (cursor != old_cursor)
			update();
	}

	size_t textinput::size() const {
		return buffer.size();
	}

	char textinput::prev_char() const {
		return cursor > 0? buffer[cursor - 1] : '\0';
	}

	char textinput::next_char() const {
		return cursor < size()? buffer[cursor] : '\0';
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
