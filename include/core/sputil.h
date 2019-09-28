#ifndef SPJALLA_CORE_SPUTIL_H_
#define SPJALLA_CORE_SPUTIL_H_

#include <filesystem>
#include <locale>
#include <string>

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

		/** Returns a path to the user's home directory as a string ending with a slash. */
		std::string get_home_string();

		/** Returns a path to the user's home directory. */
		std::filesystem::path get_home();

		/** Escapes a string by prepending all backslashes, newlines, carriage returns, tabs, nulls and double
		 *  quotes with backslashes. */
		std::string escape(const std::string &);

		/** Unescapes a string (see spjalla::config::escape). If check_dquotes are true, the function will throw a
		 *  std::invalid_argument exception if it finds an unescaped double quote. */
		std::string unescape(const std::string &, const bool check_dquotes = true);

		/** Trims spaces and tabs from both ends of a string. */
		void trim(std::string &str);
	}
}

#endif
