#include <iostream>
#include <functional>
#include <ostream>
#include <string>

#include "ui/textinput.h"
#include "lib/utf8.h"

namespace spjalla {
	void textinput::update() {
		if (on_update)
			on_update(buffer, cursor);
	}

	void textinput::listen(const update_fn &fn) {
		on_update = fn;
	}

	void textinput::move_to(size_t new_cursor) {
		if (new_cursor <= size() && new_cursor != cursor) {
			cursor = new_cursor;
			update();
		}
	}

	void textinput::insert(const std::string &str) {
		utf8str newstr = str;
		buffer.insert(cursor, newstr);
		cursor += newstr.size();
		update();
	}

	void textinput::insert(char ch) {
		if (!unicode_buffer.empty()) {
			std::cerr << "foo" << std::endl;
			unicode_buffer.push_back(ch);
			// if (utf8::is_valid(unicode_buffer.begin(), unicode_buffer.end())) {
			if (!icu::UnicodeString::fromUTF8(unicode_buffer).isBogus()) {
				// The unicode buffer now contains a complete and valid codepoint, so we insert it into the buffer.
				buffer.insert(cursor, unicode_buffer);
				unicode_buffer.clear();
				++cursor;
				update();
			} else if (unicode_buffer.size() == 4) {
				// At this point, it seems there's just garbage in the buffer. Insert it.
				buffer.insert(cursor, unicode_buffer);
				unicode_buffer.clear();
				cursor += unicode_buffer.length();
				update();
			}
		} else {
			// std::string str(1, ch);
			// unsigned char zext = static_cast<unsigned char>(ch);
			// std::cerr << std::hex << "\'" << ch << "\': " << std::hex << static_cast<unsigned int>(zext) << std::dec << std::endl;
			unsigned char uch = static_cast<unsigned char>(ch);
			if (uch < 0x80) {
				std::cerr << "Not incomplete: '" << std::string(1, ch) << "' (0x" << std::hex << static_cast<int>(uch) << std::dec << ")" << std::endl;
				buffer.insert(cursor++, ch);
				update();
				return;
			}
			
			bytes_expected = utf8::width(uch);

			if (bytes_expected == 0) {
				// This doesn't appear to be a valid UTF8 start byte. Treat it as an ASCII character.
				buffer.insert(cursor++, ch);
				update();
			} else {
				unicode_buffer.push_back(ch);
				std::cerr << "Incomplete: '" << std::string(1, ch) << "' (0x" << std::hex << static_cast<int>(uch) << std::dec << "), expecting " << bytes_expected << std::endl;
			}
		}
	}

	bool textinput::is_incomplete(const std::string &str) {
		UChar32 word = 0;
		for (size_t i = 0; i < 4 && i < str.length(); ++i)
			word |= str[i] << (i << 3);
		return icu::UnicodeString(word).isBogus();
		// return icu::UnicodeString::fromUTF8(str).isBogus();
	}

	bool textinput::is_incomplete(char ch) {
		return is_incomplete(std::string(1, ch));
		// return icu::UnicodeString(static_cast<UChar32>(static_cast<uint8_t>(ch))).isBogus(); 
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
		buffer = icu::UnicodeString::fromUTF8(str);
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
		if (cursor != buffer.length()) {
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

	size_t textinput::length() const {
		return buffer.length();
	}

	size_t textinput::size() const {
		return buffer.length();
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
		return std::string(buffer);
	}

	std::ostream & operator<<(std::ostream &os, const textinput &input) {
		os << std::string(input);
		return os;
	}
}
