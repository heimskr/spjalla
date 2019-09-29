#ifndef SPJALLA_CORE_SPUTIL_H_
#define SPJALLA_CORE_SPUTIL_H_

#include <filesystem>
#include <locale>
#include <sstream>
#include <string>
#include <utility>

#ifdef VSCODE
// VS Code, please stop pretending these don't exist.
namespace std { bool isalnum(char ch) { return isalnum(ch, std::locale()); } }
long strtol(const char *, char **, int);
char * getenv(const char *);
#endif

namespace spjalla {
	namespace ui {
		class window;
	}

	namespace util {
		/** Styles a window based on whether it's dead. */
		std::string colorize_if_dead(const std::string &, const ui::window *);

		/** Returns a path to the user's home directory as a string, optionally ending with a slash. */
		std::string get_home_string(bool append_slash = true);

		/** Returns a path to the user's home directory. */
		std::filesystem::path get_home();

		/** Escapes a string by prepending all backslashes, newlines, carriage returns, tabs, nulls and double
		 *  quotes with backslashes. */
		std::string escape(const std::string &);

		/** Unescapes a string (see spjalla::config::escape). If check_dquotes are true, the function will throw a
		 *  std::invalid_argument exception if it finds an unescaped double quote. */
		std::string unescape(const std::string &, const bool check_dquotes = true);

		/** Trims spaces and tabs from both ends of a string. */
		std::string & trim(std::string &str);
		std::string trim(const std::string &str);

		template <typename Iter>
		std::string join(Iter begin, Iter end, const std::string &delim = " ") {
			std::ostringstream oss;
			bool first = true;
			while (begin != end) {
				if (!first) {
					oss << delim;
				} else {
					first = false;
				}

				oss << *begin;
				++begin;
			}

			return oss.str();
		}

		/** Returns the index of the word that a given index is in in addition to the index within the word.
		 *  If the cursor is within a group of multiple spaces between two words, the first value will be negative.
		 *  If the first value is -1, the cursor is before the first word. -2 indicates that the cursor is before the
		 *  second word, -3 before the third, and so on. The second value will be -1 if the first value is negative. */
		std::pair<ssize_t, ssize_t> word_indices(const std::string &, size_t);
	}
}

#endif
