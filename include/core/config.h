#ifndef SPJALLA_CORE_CONFIG_H_
#define SPJALLA_CORE_CONFIG_H_

#include <string>
#include <utility>

#include "haunted/core/key.h"

namespace spjalla {
	struct keys {
		static haunted::key toggle_overlay, switch_server, next_window, previous_window;
	};

	class config {
		private:
			/** Creates a config directory in the user's home directory if one doesn't already exist.
			 *  Returns true if the directory had to be created. */
			static bool ensure_config_dir(const std::string &name = ".spjalla");

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

		public:
			enum class line_type {long_line, double_line, string_line};

			/** Escapes a string by prepending all backslashes, newlines, carriage returns, tabs, nulls and double
			 *  quotes with backslashes. */
			static std::string escape(const std::string &);

			/** Unescapes a string (see spjalla::config::escape). If check_dquotes are true, the function will throw a
			 *  std::invalid_argument exception if it finds an unescaped double quote. */
			static std::string unescape(const std::string &, const bool check_dquotes = true);

			/** Trims spaces and tabs from both ends of a string. */
			static void trim(std::string &str);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);
	};
}

#endif
