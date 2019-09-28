#ifndef SPJALLA_CORE_CONFIG_H_
#define SPJALLA_CORE_CONFIG_H_

#include <string>
#include <unordered_map>
#include <utility>

#include "haunted/core/key.h"

namespace haunted::tests { class testing; }
namespace spjalla::tests { void test_config(haunted::tests::testing &); }

namespace spjalla {
	struct keys {
		static haunted::key toggle_overlay, switch_server, next_window, previous_window;
	};

	enum class config_type {invalid, long_, double_, string_};

	class config_value {
		// Boost? Never heard of it.
		private:
			config_type type;
			long long_value {0};
			double double_value {0.};
			std::string string_value {};

		public:
			config_value(long long_):     type(config_type::long_),   long_value(long_) {}
			config_value(double double_): type(config_type::double_), double_value(double_) {}
			config_value(const std::string &str_): type(config_type::string_), string_value(str_) {}

			config_type get_type() { return type; }
			long & long_();
			double & double_();
			std::string & string_();
	};

	/**
	 * Represents an instance of a configuration database.
	 */
	class config {
		using   submap = std::unordered_map<std::string, config_value>;
		using groupmap = std::unordered_map<std::string, submap>;

		private:
			groupmap db {};

			/** Stores known option keys (the first element of the pair) under named groups (the key type of the
			 *  unordered_map) with a config_value indicating the type and default value. */
			static groupmap registered;

			/** Creates a config directory in the user's home directory if one doesn't already exist.
			 *  Returns true if the directory had to be created. */
			static bool ensure_config_dir(const std::string &name = ".spjalla");

			/** Attempts to parse a keyvalue pair of the form /^(\w+)=(.+)$/. */
			static std::pair<std::string, std::string> parse_kv_pair(const std::string &);

			/** Attempts to parse a string from a key-value pair. */
			static std::string parse_string(const std::string &);

		public:
			config() {}

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+$/. */
			static std::pair<std::string, long> parse_long_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*\d+\.\d*$/. */
			static std::pair<std::string, double> parse_double_line(const std::string &);

			/** Attempts to parse a configuration line of the form /^\w+\s*=\s*("[^\\\n\r\t\0"]*")?$/. */
			static std::pair<std::string, std::string> parse_string_line(const std::string &);

			/** Checks a value and returns its type. */
			static config_type get_value_type(std::string) noexcept;

			/** Attempts to register a key. If the key already exists, the function simply returns false; otherwise, it
			 *  registers the key and returns true. */
			static bool register_key(const std::string &group, const std::string &key, const config_value &default_val);

			friend void spjalla::tests::test_config(haunted::tests::testing &);
	};
}

#endif
