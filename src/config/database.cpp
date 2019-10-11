#include <fstream>
#include <locale>
#include <sstream>
#include <stdexcept>

#include <cstdlib>

#include "spjalla/core/util.h"
#include "spjalla/config/config.h"
#include "spjalla/config/defaults.h"

namespace spjalla::config {

// Private static methods


	bool database::parse_bool(const std::string &str) {
		return str == "true" || str == "on" || str == "yes";
	}


// Private instance methods


	void database::ensure_known(const std::string &group, const std::string &key) const {
		if (!allow_unknown && !key_known(group, key))
			throw std::invalid_argument("Unknown group+key pair");
	}


// Public static methods


	std::pair<std::string, long> database::parse_long_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		const char *value_cstr = value.c_str();
		char *end;
		long parsed = strtol(value_cstr, &end, 10);
		if (end != value_cstr + value.size())
			throw std::invalid_argument("Invalid value in key-value pair; expected a long");

		return {key, parsed};
	}

	std::pair<std::string, double> database::parse_double_line(const std::string &str) {
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

	std::pair<std::string, bool> database::parse_bool_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		if (value == ".")
			return {key, 0};

		size_t idx;
		bool parsed = std::stod(value, &idx);
		if (idx != value.length())
			throw std::invalid_argument("Invalid value in key-value pair; expected a bool");

		return {key, parsed};
	}

	std::pair<std::string, std::string> database::parse_pair(const std::string &str) {
		size_t period = str.find('.');
		if (period == std::string::npos || period == 0 || period == str.length() - 1 || period != str.find_last_of("."))
			throw std::invalid_argument("Invalid group+key pair");
		return {str.substr(0, period), str.substr(period + 1)};
	}

	value_type database::get_value_type(std::string val) noexcept {
		util::trim(val);

		if (val.empty())
			return value_type::string_;

		if (val.find_first_not_of("0123456789") == std::string::npos)
			return value_type::long_;

		if (val.find_first_not_of("0123456789.") == std::string::npos) {
			// Don't allow multiple periods in the string.
			if (val.find('.') != val.find_last_of("."))
				return value_type::invalid;
			return value_type::double_;
		}

		if (val == "true" || val == "false" || val == "on" || val == "off" || val == "yes" || val == "no")
			return value_type::bool_;

		if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
			return value_type::string_;

		return value_type::invalid;
	}


// Public instance methods


	value & database::get(const std::string &group, const std::string &key) {
		ensure_known(group, key);

		if (has_key(group, key))
			return db.at(group).at(key);

		if (key_known(group, key))
			return registered.at(group + "." + key).default_value;

		throw std::out_of_range("No value for group+key pair");
	}

	bool database::insert(const std::string &group, const std::string &key, const value &value, bool save) {
		ensure_known(group, key);

		submap &sub = db[group];
		bool overwritten = false;

		const auto iter = registered.find(group + "." + key);
		const bool is_registered = iter != registered.end();
		if (is_registered) {
			validation_result result = iter->second.validate(value);
			if (result != validation_result::valid)
				throw validation_failure(result);
		}

		if (sub.count(key) > 0) {
			sub.erase(key);
			overwritten = true;
		}

		sub.insert({key, value});

		if (save)
			write_db();

		if (is_registered)
			iter->second.apply(*this, value);

		return overwritten;
	}

	bool database::insert_any(const std::string &group, const std::string &key, const std::string &value, bool save) {
		const value_type type = database::get_value_type(value);
		switch (type) {
			case value_type::long_:   return insert(group, key, {strtol(value.c_str(), nullptr, 10)}, save);
			case value_type::double_: return insert(group, key, {std::stod(value)}, save);
			case value_type::bool_:   return insert(group, key, {parse_bool(value)}, save);
			case value_type::string_: return insert(group, key, {database::parse_string(value)}, save);
			default:
				throw std::invalid_argument("Invalid value type");
		}
	}

	bool database::remove(const std::string &group, const std::string &key, bool apply_default, bool save) {
		if (!has_key(group, key))
			return false;

		db[group].erase(key);

		if (apply_default) {
			const std::string combined {group + "." + key};
			if (registered.count(combined) == 1)
				registered.at(combined).apply(*this);
		}

		if (save)
			write_db();

		return true;
	}

	std::pair<std::string, std::string> database::apply_line(const std::string &line) {
		std::string group, key, gk, value;
		std::tie(gk, value) = parse_kv_pair(line);
		std::tie(group, key) = parse_pair(gk);
		insert_any(group, key, value);
		return {group + "." + key, value};
	}

	void database::apply_all(bool with_defaults) {
		for (auto &pair: registered) {
			std::string group, key;
			std::tie(group, key) = parse_pair(pair.first);
			if (has_key(group, key)) {
				pair.second.apply(*this, get(group, key));
			} else if (with_defaults) {
				pair.second.apply(*this);
			}
		}
	}

	bool database::has_group(const std::string &group) const {
		return db.count(group) > 0;
	}

	bool database::has_key(const std::string &group, const std::string &key) const {
		return has_group(group) && db.at(group).count(key) > 0;
	}

	bool database::key_known(const std::string &group, const std::string &key) const {
		return registered.count(group + "." + key) > 0;
	}

	ssize_t database::key_count(const std::string &group) const {
		return has_group(group)? db.at(group).size() : -1;
	}

	database::groupmap database::with_defaults() const {
		groupmap copy {db};
		for (const auto &gpair: registered) {
			const default_key &def = gpair.second;

			std::string group, key;
			const std::string &combined = gpair.first;
			std::tie(group, key) = parse_pair(combined);

			copy[group].insert({key, def.default_value});
		}

		return copy;
	}

	database::operator std::string() const {
		std::ostringstream out;
		for (const auto &gpair: db) {
			const std::string &group = gpair.first;
			const submap &sub = gpair.second;

			for (const auto &spair: sub) {
				const std::string &key = spair.first;
				const value &value = spair.second;
				out << group << "." << key << "=" << value.escaped() << "\n";
			}
		}

		return out.str();
	}
}
