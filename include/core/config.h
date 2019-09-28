#ifndef SPJALLA_CORE_CONFIG_H_
#define SPJALLA_CORE_CONFIG_H_

#include <string>
#include <unordered_map>
#include <utility>

#include "haunted/core/key.h"

namespace spjalla {
	struct keys {
		static haunted::key toggle_overlay, switch_server, next_window, previous_window;
	};

	enum class config_type {long_, double_, string_, invalid};

	class config_value {
		// Boost? Never heard of it.
		private:
			config_type type;
			long long_value {0};
			double double_value {0.};
			std::string string_value {};

		public:
			config_value(long long_):       type(config_type::long_),   long_value(long_) {}
			config_value(double double_):   type(config_type::double_), double_value(double_) {}
			config_value(std::string str_): type(config_type::string_), string_value(str_) {}

			long & long_();
			double & double_();
			std::string & string_();
	};

	class config {
		private:
			/** Creates a config directory in the user's home directory if one doesn't already exist.
			 *  Returns true if the directory had to be created. */
			static bool ensure_config_dir(const std::string &name = ".spjalla");

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

			/** Attempts to parse a string from a key-value pair. */
			static std::string parse_string(const std::string &);

			std::unordered_map<std::string, config_value> db;

		public:

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);

			/** Checks a line and returns its type. */
			static config_type get_line_type(const std::string &) noexcept;
	};
}

#endif
