#ifndef SPJALLA_CORE_SPUTIL_H_
#define SPJALLA_CORE_SPUTIL_H_

#include <algorithm>
#include <filesystem>
#include <list>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "lib/formicine/ansi.h"
#include "lib/formicine/futil.h"

#ifdef VSCODE
// VS Code, please stop pretending these don't exist.
namespace std { bool isalnum(char ch) { return isalnum(ch, std::locale()); } }
long strtol(const char *, char **, int);
char * getenv(const char *);
#endif

namespace Spjalla {
	namespace UI {
		class Window;
	}

	namespace Util {
		/** Styles a window based on whether it's dead. */
		std::string colorizeIfDead(const std::string &, const UI::Window *);

		/** Returns a path to the user's home directory as a string, optionally ending with a slash. */
		std::string getHomeString(bool append_slash = true);

		/** Returns a path to the user's home directory. */
		std::filesystem::path getHome();

		/** Escapes a string by prepending all backslashes, newlines, carriage returns, tabs, nulls and double
		 *  quotes with backslashes. */
		std::string escape(const std::string &);

		/** Unescapes a string (see spjalla::config::escape). If check_dquotes are true, the function will throw a
		 *  std::invalid_argument exception if it finds an unescaped double quote. */
		std::string unescape(const std::string &, const bool check_dquotes = true);

		/** Determines whether a message is a highlight for a given name. */
		bool isHighlight(const std::string &message, const std::string &name, bool direct_only);

		auto constexpr lower   = &formicine::util::lower;
		auto constexpr upper   = &formicine::util::upper;
		auto constexpr split   = &formicine::util::split;
		auto constexpr nthWord = &formicine::util::nth_word;
	}
}

#endif
