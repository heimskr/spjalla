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
			char ch = i;

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
}
