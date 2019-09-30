#include "spjalla/core/util.h"

#include "spjalla/config/value.h"

namespace spjalla::config {
	long & value::long_ref() {
		if (type != value_type::long_)
			throw std::runtime_error("Underlying type of value isn't long");
		return long_value;
	}

	double & value::double_ref() {
		if (type != value_type::double_)
			throw std::runtime_error("Underlying type of value isn't double");
		return double_value;
	}

	std::string & value::string_ref() {
		if (type != value_type::string_)
			throw std::runtime_error("Underlying type of value isn't string");
		return string_value;
	}

	long value::long_() const {
		if (type != value_type::long_)
			throw std::runtime_error("Underlying type of value isn't long");
		return long_value;
	}

	double value::double_() const {
		if (type != value_type::double_)
			throw std::runtime_error("Underlying type of value isn't double");
		return double_value;
	}

	const std::string & value::string_() const {
		if (type != value_type::string_)
			throw std::runtime_error("Underlying type of value isn't string");
		return string_value;
	}

	value & value::operator=(long new_long) {
		type = value_type::long_;
		long_value = new_long;
		return *this;
	}

	value & value::operator=(double new_double) {
		type = value_type::double_;
		double_value = new_double;
		return *this;
	}

	value & value::operator=(const std::string &new_string) {
		type = value_type::string_;
		string_value = new_string;
		return *this;
	}

	bool value::operator==(const value &other) const {
		return type == other.type && ((type == value_type::long_ && long_value == other.long_value) ||
			(type == value_type::double_ && double_value == other.double_value) ||
			(type == value_type::string_ && string_value == other.string_value));
	}

	bool value::operator==(long other) const {
		return type == value_type::long_ && long_value == other;
	}

	bool value::operator==(double other) const {
		return type == value_type::double_ && double_value == other;
	}

	bool value::operator==(const std::string &other) const {
		return type == value_type::string_ && string_value == other;
	}

	value::operator std::string() const {
		if (type == value_type::long_)
			return std::to_string(long_value);
		if (type == value_type::double_)
			return std::to_string(double_value);
		if (type == value_type::string_)
			return string_value;
		throw std::invalid_argument("Invalid value type");
	}

	std::string value::escaped() const {
		if (type == value_type::string_)
			return "\"" + util::escape(string_value) + "\"";
		return std::string(*this);
	}

	std::ostream & operator<<(std::ostream &os, const value &value) {
		return os << std::string(value);
	}
}
