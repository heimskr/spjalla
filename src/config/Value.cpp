#include "spjalla/core/Util.h"

#include "spjalla/config/Value.h"

namespace Spjalla::Config {
	long & Value::longRef() {
		if (type != ValueType::Long)
			throw std::runtime_error("Underlying type of value isn't long");
		return longValue;
	}

	double & Value::doubleRef() {
		if (type != ValueType::Double)
			throw std::runtime_error("Underlying type of value isn't double");
		return doubleValue;
	}

	bool & Value::boolRef() {
		if (type != ValueType::Bool)
			throw std::runtime_error("Underlying type of value isn't bool");
		return boolValue;
	}

	std::string & Value::stringRef() {
		if (type != ValueType::String)
			throw std::runtime_error("Underlying type of value isn't string");
		return stringValue;
	}

	long Value::long_() const {
		if (type != ValueType::Long)
			throw std::runtime_error("Underlying type of value isn't long");
		return longValue;
	}

	double Value::double_() const {
		if (type != ValueType::Double)
			throw std::runtime_error("Underlying type of value isn't double");
		return doubleValue;
	}

	bool Value::bool_() const {
		if (type != ValueType::Bool)
			throw std::runtime_error("Underlying type of value isn't bool");
		return boolValue;
	}

	const std::string & Value::string_() const {
		if (type != ValueType::String)
			throw std::runtime_error("Underlying type of value isn't string");
		return stringValue;
	}

	Value & Value::operator=(long new_long) {
		type = ValueType::Long;
		longValue = new_long;
		return *this;
	}

	Value & Value::operator=(double new_double) {
		type = ValueType::Double;
		doubleValue = new_double;
		return *this;
	}

	Value & Value::operator=(bool new_bool) {
		type = ValueType::Bool;
		boolValue = new_bool;
		return *this;
	}

	Value & Value::operator=(const std::string &new_string) {
		type = ValueType::String;
		stringValue = new_string;
		return *this;
	}

	bool Value::operator==(const Value &other) const {
		return type == other.type && ((type == ValueType::Long && longValue == other.longValue) ||
			(type == ValueType::Double && doubleValue == other.doubleValue) ||
			(type == ValueType::String && stringValue == other.stringValue));
	}

	bool Value::operator==(long other) const {
		return type == ValueType::Long && longValue == other;
	}

	bool Value::operator==(double other) const {
		return type == ValueType::Double && doubleValue == other;
	}

	bool Value::operator==(bool other) const {
		return type == ValueType::Bool && boolValue == other;
	}

	bool Value::operator==(const std::string &other) const {
		return type == ValueType::String && stringValue == other;
	}

	Value::operator std::string() const {
		if (type == ValueType::Long)
			return std::to_string(longValue);
		if (type == ValueType::Double)
			return std::to_string(doubleValue);
		if (type == ValueType::String)
			return stringValue;
		if (type == ValueType::Bool)
			return boolValue? "true" : "false";
		throw std::invalid_argument("Invalid value type");
	}

	std::string Value::escaped() const {
		if (type == ValueType::String)
			return "\"" + Util::escape(stringValue) + "\"";
		return std::string(*this);
	}
}

std::ostream & operator<<(std::ostream &os, const Spjalla::Config::Value &value) {
	return os << std::string(value);
}
