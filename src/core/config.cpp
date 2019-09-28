#include <fstream>
#include <locale>
#include <sstream>
#include <stdexcept>

#include <cstdlib>

#include "core/config.h"
#include "core/sputil.h"

namespace spjalla {
	haunted::key keys::toggle_overlay  = {haunted::ktype::semicolon, haunted::kmod::ctrl};
	haunted::key keys::switch_server   = {haunted::ktype::x,         haunted::kmod::ctrl};
	haunted::key keys::next_window     = {haunted::ktype::n,         haunted::kmod::ctrl};
	haunted::key keys::previous_window = {haunted::ktype::p,         haunted::kmod::ctrl};

	long & config_value::long_() {
		if (type != config_type::long_)
			throw std::runtime_error("Underlying type of config_value isn't long");
		return long_value;
	}

	double & config_value::double_() {
		if (type != config_type::double_)
			throw std::runtime_error("Underlying type of config_value isn't double");
		return double_value;
	}

	std::string & config_value::string_() {
		if (type != config_type::string_)
			throw std::runtime_error("Underlying type of config_value isn't string");
		return string_value;
	}

	config_value & config_value::operator=(long new_long) {
		type = config_type::long_;
		long_value = new_long;
		return *this;
	}

	config_value & config_value::operator=(double new_double) {
		type = config_type::double_;
		double_value = new_double;
		return *this;
	}

	config_value & config_value::operator=(const std::string &new_string) {
		type = config_type::string_;
		string_value = new_string;
		return *this;
	}

	bool config_value::operator==(const config_value &other) const {
		return type == other.type && ((type == config_type::long_ && long_value == other.long_value) ||
			(type == config_type::double_ && double_value == other.double_value) ||
			(type == config_type::string_ && string_value == other.string_value));
	}

	bool config_value::operator==(long other) const {
		return type == config_type::long_ && long_value == other;
	}

	bool config_value::operator==(double other) const {
		return type == config_type::double_ && double_value == other;
	}

	bool config_value::operator==(const std::string &other) const {
		return type == config_type::string_ && string_value == other;
	}

	config_value::operator std::string() const {
		if (type == config_type::long_)
			return std::to_string(long_value);
		if (type == config_type::double_)
			return std::to_string(double_value);
		if (type == config_type::string_)
			return "\"" + util::escape(string_value) + "\"";
		throw std::invalid_argument("Invalid config_value type");
	}

	std::ostream & operator<<(std::ostream &os, const config_value &value) {
		return os << std::string(value);
	}


// Private static fields (config)


	config::groupmap config::registered = {};


// Private static methods (config)


	bool config::ensure_config_dir(const std::string &name) {
		std::filesystem::path config_path = util::get_home() / name;
		if (!std::filesystem::exists(config_path)) {
			std::filesystem::create_directory(config_path);
			return true;
		}

		return false;
	}

	bool config::ensure_config_db(const std::string &dbname, const std::string &dirname) {
		ensure_config_dir(dirname);
		std::filesystem::path db_path = get_db_path(dbname, dirname);

		if (!std::filesystem::exists(db_path)) {
			std::ofstream(db_path).close();
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

		if (equals == 0 || equals == str.find_first_not_of(" "))
			throw std::invalid_argument("Empty key in key-value pair");

		std::string key = str.substr(0, equals);
		util::trim(key);

		for (char ch: key) {
			if (!std::isalnum(ch))
				throw std::invalid_argument("Key isn't alphanumeric in key-value pair");
		}

		return {key, util::trim(str.substr(equals + 1))};
	}

	std::string config::parse_string(std::string value) {
		util::trim(value);
		const size_t vlength = value.length();

		// Special case: an empty value represents an empty string, same as a pair of double quotes.
		if (vlength == 0)
			return "";

		if (vlength < 2)
			throw std::invalid_argument("Invalid length of string value");

		if (value.front() != '"' || value.back() != '"')
			throw std::invalid_argument("Invalid quote placement in string value");

		return util::unescape(value.substr(1, vlength - 2));
	}

	std::filesystem::path config::get_db_path(const std::string &dbname, const std::string &dirname) {
		return util::get_home() / dirname / dbname;
	}


// Private instance methods (config)


	void config::ensure_known(const std::string &group, const std::string &key) const {
		if (!allow_unknown && !key_known(group, key))
			throw std::invalid_argument("Unknown group+key pair");
	}

	void config::write_db() {
		
	}

	void config::read_db() {
		
	}


// Public static methods (config)


	std::pair<std::string, long> config::parse_long_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		const char *value_cstr = value.c_str();
		char *end;
		long parsed = strtol(value_cstr, &end, 10);
		if (end != value_cstr + value.size())
			throw std::invalid_argument("Invalid value in key-value pair; expected a long");

		return {key, parsed};
	}

	std::pair<std::string, double> config::parse_double_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		if (value == ".")
			return {key, 0};

		size_t idx;
		double parsed = std::stod(value, &idx);
		if (idx != value.length())
			throw std::invalid_argument("Invalid value in key-value pair; expected a double");

		return {key, parsed};
	}

	std::pair<std::string, std::string> config::parse_string_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		return {key, parse_string(value)};
	}

	std::pair<std::string, std::string> config::parse_pair(const std::string &str) {
		size_t period = str.find('.');
		if (period == std::string::npos || period == 0 || period == str.length() - 1 || period != str.find_last_of("."))
			throw std::invalid_argument("Invalid group+key pair");
		return {str.substr(0, period), str.substr(period + 1)};
	}

	config_type config::get_value_type(std::string value) noexcept {
		util::trim(value);

		if (value.empty())
			return config_type::string_;

		if (value.find_first_not_of("0123456789") == std::string::npos)
			return config_type::long_;

		if (value.find_first_not_of("0123456789.") == std::string::npos) {
			// Don't allow multiple periods in the string.
			if (value.find('.') != value.find_last_of("."))
				return config_type::invalid;
			return config_type::double_;
		}

		if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
			return config_type::string_;

		return config_type::invalid;
	}

	bool config::register_key(const std::string &group, const std::string &key, const config_value &default_value) {
		config::submap &keys = registered[group];
		if (keys.count(key) > 0)
			return false;

		keys.insert({key, default_value});
		return true;
	}


// Public instance methods (config)


	void config::set_path(const std::string &dbname, const std::string &dirname) {
		ensure_config_db(dbname, dirname);
		filepath = get_db_path(dbname, dirname);
		read_db();
	}

	bool config::insert(const std::string &group, const std::string &key, const config_value &value) {
		ensure_known(group, key);

		submap &sub = db[group];
		bool overwritten = false;

		if (sub.count(key) > 0) {
			sub.erase(key);
			overwritten = true;
		}

		sub.insert({key, value});
		return overwritten;
	}

	config_value & config::get(const std::string &group, const std::string &key) {
		ensure_known(group, key);

		if (has_key(group, key))
			return db.at(group).at(key);

		if (key_known(group, key))
			return registered.at(group).at(key);

		throw std::out_of_range("No value for group+key pair");
	}

	bool config::has_group(const std::string &group) const {
		return db.count(group) > 0;
	}

	bool config::has_key(const std::string &group, const std::string &key) const {
		return has_group(group) && db.at(group).count(key) > 0;
	}

	bool config::key_known(const std::string &group, const std::string &key) const {
		return registered.count(group) > 0 && registered.at(group).count(key) > 0;
	}

	ssize_t config::key_count(const std::string &group) const {
		return has_group(group)? db.at(group).size() : -1;
	}

	config::operator std::string() const {
		std::ostringstream out;
		for (const auto &gpair: db) {
			const std::string &group = gpair.first;
			const submap &sub = gpair.second;

			for (const auto &spair: sub) {
				const std::string &key = spair.first;
				const config_value &value = spair.second;
				out << group << "." << key << "=" << value << "\n";
			}
		}

		return out.str();
	}
}
