#include <locale>
#include <sstream>

#include <cstdlib>

#include "core/config.h"
#include "core/sputil.h"

namespace spjalla {
	haunted::key keys::toggle_overlay  = {haunted::ktype::semicolon, haunted::kmod::ctrl};
	haunted::key keys::switch_server   = {haunted::ktype::x,         haunted::kmod::ctrl};
	haunted::key keys::next_window     = {haunted::ktype::n,         haunted::kmod::ctrl};
	haunted::key keys::previous_window = {haunted::ktype::p,         haunted::kmod::ctrl};

	bool config::ensure_config_dir(const std::string &name) {
		std::filesystem::path config_path = util::get_home() / name;
		if (!std::filesystem::exists(config_path)) {
			std::filesystem::create_directory(config_path);
			return true;
		}

		return false;
	}

	std::pair<std::string, std::string> config::parse_kv_pair(const std::string &str) {
		if (str.empty())
			throw std::invalid_argument("Can't parse empty string as key-value pair");

		const size_t equals = str.find('=');

		if (equals == std::string::npos)
			throw std::invalid_argument("No equals sign found in key-value pair");

		if (equals == 0)
			throw std::invalid_argument("Empty key in key-value pair");

		if (equals == str.length() - 1)
			throw std::invalid_argument("Empty value in key-value pair");

		std::string key = str.substr(0, equals);
		for (char ch: key) {
			if (!std::isalnum(ch))
				throw std::invalid_argument("Key isn't alphanumeric in key-value pair");
		}

		return {key, str.substr(equals + 1)};
	}

	std::string config::escape(const std::string &str) {
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

	std::string config::unescape(const std::string &str) {
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
			} else {
				out << ch;
			}
		}

		return out.str();
	}

	std::pair<std::string, long> config::parse_long_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);
		const char *value_cstr = value.c_str();
		const char *end;
		long parsed = strtol(value_cstr, &end, 10);
		if (end != value_cstr + value.size())
			throw std::invalid_argument("Invalid value in key-value pair; expected a long");

		return {key, parsed};
	}
}
