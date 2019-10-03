#include <cstdlib>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "spjalla/core/util.h"
#include "spjalla/ui/window.h"
#include "lib/formicine/futil.h"

namespace spjalla::util {
	std::string colorize_if_dead(const std::string &str, const ui::window *win) {
		return win->data.dead? ansi::red(str) : str;
	}

	std::string get_home_string(bool append_slash) {
		const char *home = getenv("HOME");
		if (home == nullptr)
			home = getpwuid(getuid())->pw_dir;
		std::string str {home};
		if (append_slash && !str.empty() && str.back() != '/' && str.back() != '\\')
			str.push_back('/');
		return str;
	}

	std::filesystem::path get_home() {
		return {get_home_string(false)};
	}

	std::string escape(const std::string &str) {
		std::ostringstream out;
		for (char ch: str) {
			switch (ch) {
				case '\\': out << "\\\\"; break;
				case '\n': out << "\\n"; break;
				case '\r': out << "\\r"; break;
				case '\t': out << "\\t"; break;
				case '\0': out << "\\0"; break;
				case '"':  out << "\""; break;
				default:   out << ch;
			}
		}

		return out.str();
	}

	std::string unescape(const std::string &str, const bool check_dquotes) {
		std::ostringstream out;
		for (size_t i = 0, length = str.length(); i < length; ++i) {
			char ch = str[i];

			// Looking at the next character when we're at the end of the string would be bad.
			if (i == length - 1) {
				out << ch;
				break;
			}

			if (ch == '\\') {
				switch (str[i + 1]) {
					case '\\': out << "\\"; ++i; break;
					case 'n':  out << "\n"; ++i; break;
					case 'r':  out << "\r"; ++i; break;
					case 't':  out << "\t"; ++i; break;
					case '0':  out << "\0"; ++i; break;
					case '"':  out << "\""; ++i; break;
				}
			} else if (check_dquotes && ch == '"') {
				throw std::invalid_argument("String contains an unescaped double quote");
			} else {
				out << ch;
			}
		}

		return out.str();
	}

	std::string & trim(std::string &str) {
		if (str.empty())
			return str;

		char ch = str.back();
		while (ch == ' ' || ch == '\t') {
			str.pop_back();
			ch = str.back();
		}

		if (str.empty())
			return str;

		const size_t first = str.find_first_not_of(" \t");
		if (first == std::string::npos)
			str.clear();
		else
			str.erase(0, first);

		return str;
	}

	std::string trim(const std::string &str) {
		std::string copy {str};
		return trim(copy);
	}

	std::pair<ssize_t, ssize_t> word_indices(const std::string &str, size_t cursor) {
		const size_t length = str.length();

		ssize_t word_index = -1;
		ssize_t sub_index  = -1;
		char prev_char = '\0';

		if (str.empty())
			return {-1, -1};

		if (cursor == 0)
			return str[0] == ' '? std::pair(-1, -1) : std::pair(0, 0);

		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch != ' ' && (prev_char == '\0' || prev_char == ' ')) {
				++word_index;
				sub_index = 0;
			}

			if (ch == ' ') {
				if (prev_char != ' ') {
					// We've reached the end of a word. If this is where the cursor is, we're done.
					// Otherwise, it's time to increment the word index.
					if (i == cursor)
						return {word_index, sub_index};
				} else if (i == cursor) {
					// If we're within a group of spaces and this is the where the cursor is, return a negative number
					// because that means the cursor isn't in a word.
					return {-word_index - 2, -1};
				}
			} else {
				if (i == cursor)
					return {word_index, sub_index};

				++sub_index;
			}

			prev_char = ch;
		}

		if (cursor == length && prev_char != ' ')
			return {word_index, sub_index};

		return {-word_index - 2, -1};
	}

	size_t index_of_word(const std::string &str, size_t n) {
		const size_t length = str.length();
		size_t word_index = 0;

		char prev_char = '\0';
		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch == ' ' && (prev_char != ' ' && prev_char != '\0')) {
				// We're at a space and the previous character wasn't a space (or the beginning of the string).
				// This means we've just left a word, so increment the word index.
				++word_index;
			}

			if (ch != ' ' && (prev_char == ' ' || prev_char == '\0') && word_index == n)
				return i;

			prev_char = ch;
		}

		return length;
	}

	size_t last_index_of_word(const std::string &str, size_t n) {
		const size_t length = str.length();
		size_t word_index = 0;

		char prev_char = '\0';
		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch == ' ' && (prev_char != ' ' && prev_char != '\0')) {
				// We're at a space and the previous character wasn't a space (or the beginning of the string).
				// This means we've just left a word, so check whether we're at the requested word index.
				if (word_index++ == n)
					return i;
			}

			prev_char = ch;
		}

		return length;
	}

	ssize_t replace_word(std::string &str, size_t n, const std::string &word) {
		const size_t index = index_of_word(str, n);
		const size_t original_length = str.length();
		if (index == original_length)
			return -1;

		size_t original_word_width = 0;
		for (size_t i = index; i < original_length; ++i) {
			if (std::isspace(str[i]))
				break;
			++original_word_width;
		}

		str.replace(index, original_word_width, word);
		return index + word.length();
	}

	std::string & remove_suffix(std::string &word, const std::string &suffix) {
		if (word.empty() || suffix.empty())
			return word;

		const size_t pos = word.rfind(suffix);
		if (pos != std::string::npos && pos + suffix.length() == word.length())
			word.erase(pos);

		return word;
	}

	std::string remove_suffix(const std::string &word, const std::string &suffix) {
		if (word.empty() || suffix.empty())
			return word;

		const size_t pos = word.rfind(suffix);
		if (pos != std::string::npos && pos + suffix.length() == word.length())
			return word.substr(0, pos);

		return word;
	}

	bool is_highlight(const std::string &message, const std::string &name, bool direct_only) {
		const std::string lmessage = formicine::util::lower(message);
		const std::string lname = formicine::util::lower(name);

		if (lmessage.find(lname + ":") == 0 || lmessage.find(lname + ",") == 0 || lmessage.find(lname + " ") == 0)
			return true;

		if (!direct_only) {
			std::string filtered = formicine::util::filter(lmessage, std::string(" ") + pingpong::util::nick_chars);
			std::vector<std::string> words = formicine::util::split(filtered, " ", true);
			for (const std::string &word: words) {
				if (word == lname)
					return true;
			}
		}

		return false;
	}

	bool parse_long(const std::string &str, long &out) {
		const char *c_str = str.c_str();
		char *end;
		long parsed = strtol(c_str, &end, 10);
		if (end == c_str + str.length()) {
			out = parsed;
			return true;
		}

		return false;
	}
}
