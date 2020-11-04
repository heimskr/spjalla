#include <fstream>
#include <locale>
#include <sstream>
#include <stdexcept>

#include <cstdlib>

#include "spjalla/core/Util.h"
#include "spjalla/config/Config.h"
#include "spjalla/config/Defaults.h"

namespace Spjalla::Config {

// Private static methods


	bool Database::parseBool(const std::string &str) {
		return str == "true" || str == "on" || str == "yes";
	}


// Private instance methods


	void Database::ensureKnown(const std::string &group, const std::string &key) const {
		if (!allowUnknown && !keyKnown(group, key))
			throw std::invalid_argument("Unknown group+key pair");
	}


// Public static methods


	std::pair<std::string, long> Database::parseLongLine(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parseKVPair(str);

		const char *value_cstr = value.c_str();
		char *end;
		long parsed = strtol(value_cstr, &end, 10);
		if (end != value_cstr + value.size())
			throw std::invalid_argument("Invalid value in key-value pair; expected a long");

		return {key, parsed};
	}

	std::pair<std::string, double> Database::parseDoubleLine(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parseKVPair(str);

		if (value == ".")
			return {key, 0};

		size_t idx;
		double parsed = std::stod(value, &idx);
		if (idx != value.length())
			throw std::invalid_argument("Invalid value in key-value pair; expected a double");

		return {key, parsed};
	}

	std::pair<std::string, bool> Database::parseBoolLine(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parseKVPair(str);

		if (value == ".")
			return {key, 0};

		size_t idx;
		bool parsed = std::stod(value, &idx);
		if (idx != value.length())
			throw std::invalid_argument("Invalid value in key-value pair; expected a bool");

		return {key, parsed};
	}

	std::pair<std::string, std::string> Database::parsePair(const std::string &str) {
		size_t period = str.find('.');
		if (period == std::string::npos || period == 0 || period == str.length() - 1 || period != str.find_last_of("."))
			throw std::invalid_argument("Invalid group+key pair");
		return {str.substr(0, period), str.substr(period + 1)};
	}

	ValueType Database::getValueType(std::string val) noexcept {
		formicine::util::trim(val);

		if (val.empty())
			return ValueType::String;

		if (val.find_first_not_of("0123456789") == std::string::npos)
			return ValueType::Long;

		if (val.find_first_not_of("0123456789.") == std::string::npos) {
			// Don't allow multiple periods in the string.
			if (val.find('.') != val.find_last_of("."))
				return ValueType::Invalid;
			return ValueType::Double;
		}

		if (val == "true" || val == "false" || val == "on" || val == "off" || val == "yes" || val == "no")
			return ValueType::Bool;

		if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
			return ValueType::String;

		return ValueType::Invalid;
	}


// Public instance methods


	Value & Database::get(const std::string &group, const std::string &key) {
		ensureKnown(group, key);

		if (hasKey(group, key))
			return db.at(group).at(key);

		if (keyKnown(group, key))
			return registered.at(group + "." + key).defaultValue;

		throw std::out_of_range("No value for group+key pair");
	}

	bool Database::insert(const std::string &group, const std::string &key, const Value &value, bool save) {
		ensureKnown(group, key);

		SubMap &sub = db[group];
		bool overwritten = false;

		const auto iter = registered.find(group + "." + key);
		const bool is_registered = iter != registered.end();
		if (is_registered) {
			ValidationResult result = iter->second.validate(value);
			if (result != ValidationResult::Valid)
				throw ValidationFailure(result);
		}

		if (sub.count(key) > 0) {
			sub.erase(key);
			overwritten = true;
		}

		sub.insert({key, value});

		if (save)
			writeDB();

		if (is_registered)
			iter->second.apply(*this, value);

		return overwritten;
	}

	bool Database::insertAny(const std::string &group, const std::string &key, const std::string &value, bool save) {
		const ValueType type = Database::getValueType(value);
		switch (type) {
			case ValueType::Long:   return insert(group, key, {strtol(value.c_str(), nullptr, 10)}, save);
			case ValueType::Double: return insert(group, key, {std::stod(value)}, save);
			case ValueType::Bool:   return insert(group, key, {parseBool(value)}, save);
			case ValueType::String: return insert(group, key, {Database::parseString(value)}, save);
			default:
				throw std::invalid_argument("Invalid value type");
		}
	}

	bool Database::remove(const std::string &group, const std::string &key, bool apply_default, bool save) {
		if (!hasKey(group, key))
			return false;

		db[group].erase(key);

		if (apply_default) {
			const std::string combined {group + "." + key};
			if (registered.count(combined) == 1)
				registered.at(combined).apply(*this);
		}

		if (save)
			writeDB();

		return true;
	}

	std::pair<std::string, std::string> Database::applyLine(const std::string &line) {
		if (!line.empty() && line.front() == '#')
			return {"", ""};

		std::string group, key, gk, value;
		std::tie(gk, value) = parseKVPair(line);
		std::tie(group, key) = parsePair(gk);
		insertAny(group, key, value);
		return {group + "." + key, value};
	}

	void Database::applyAll(bool with_defaults) {
		for (auto &pair: registered) {
			std::string group, key;
			std::tie(group, key) = parsePair(pair.first);
			if (hasKey(group, key)) {
				pair.second.apply(*this, get(group, key));
			} else if (with_defaults) {
				pair.second.apply(*this);
			}
		}
	}

	bool Database::hasGroup(const std::string &group) const {
		return db.count(group) > 0;
	}

	bool Database::hasKey(const std::string &group, const std::string &key) const {
		return hasGroup(group) && db.at(group).count(key) > 0;
	}

	bool Database::keyKnown(const std::string &group, const std::string &key) const {
		return registered.count(group + "." + key) > 0;
	}

	ssize_t Database::keyCount(const std::string &group) const {
		return hasGroup(group)? db.at(group).size() : -1;
	}

	Database::GroupMap Database::withDefaults() const {
		GroupMap copy {db};
		for (const auto &gpair: registered) {
			const DefaultKey &def = gpair.second;

			std::string group, key;
			const std::string &combined = gpair.first;
			std::tie(group, key) = parsePair(combined);

			copy[group].insert({key, def.defaultValue});
		}

		return copy;
	}

	Database::operator std::string() const {
		std::ostringstream out;
		for (const auto &gpair: db) {
			const std::string &group = gpair.first;
			const SubMap &sub = gpair.second;

			for (const auto &spair: sub) {
				const std::string &key = spair.first;
				const Value &value = spair.second;
				out << group << "." << key << "=" << value.escaped() << "\n";
			}
		}

		return out.str();
	}
}
