#include <cstdlib>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/sputil.h"
#include "ui/window.h"

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

	ssize_t word_index(const std::string &str, size_t cursor) {
		const size_t length = str.length();
		if (length < cursor)
			return -1;

		size_t word_index = 0;
		char prev_char = '\0';
		char next_char = '\0';
		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];
			next_char = i == length - 1? '\0' : str[i + 1];

			if (ch == ' ') {
				if (prev_char != ' ') {
					// We've reached the end of a word. If this is where the cursor is, we're done.
					// Otherwise, it's time to increment the word index.
					if (i == cursor)
						return word_index;
					++word_index;
				} else if (next_char == ' ' && i == cursor) {
					// If we're within a group of spaces and this is the where the cursor is, return -1 because that
					// means the cursor isn't in a word.
					return -1;
				}
			} else if (cursor == i) {
				return word_index;
			}

			prev_char = ch;
		}

		if (cursor == length)
			return word_index;

		return -1;
	}
}
