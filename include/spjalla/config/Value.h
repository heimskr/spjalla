#ifndef SPJALLA_CONFIG_VALUE_H_
#define SPJALLA_CONFIG_VALUE_H_

#include <string>

namespace Spjalla::Config {
	enum class ValueType {Invalid, Long, Double, String, Bool};

	class Value {
		// Boost? Never heard of it.
		private:
			ValueType type;
			long longValue {0};
			double doubleValue {0.};
			bool boolValue {false};
			std::string stringValue {};

		public:
			Value(long long_):     type(ValueType::Long),   longValue(long_) {}
			Value(double double_): type(ValueType::Double), doubleValue(double_) {}
			Value(bool bool_):     type(ValueType::Bool),   boolValue(bool_) {}
			Value(const std::string &str_): type(ValueType::String), stringValue(str_) {}
			Value(int int_):         Value(static_cast<long>(int_)) {}
			Value(const char *str_): Value(std::string(str_)) {}

			ValueType getType() const { return type; }
			long & longRef();
			double & doubleRef();
			bool & boolRef();
			std::string & stringRef();

			long long_() const;
			double double_() const;
			bool bool_() const;
			const std::string & string_() const;

			bool isLong()   const { return type == ValueType::Long;   }
			bool isDouble() const { return type == ValueType::Double; }
			bool isBool()   const { return type == ValueType::Bool;   }
			bool isString() const { return type == ValueType::String; }

			Value & operator=(long);
			Value & operator=(double);
			Value & operator=(bool);
			Value & operator=(const std::string &);
			Value & operator=(int n) { return *this = static_cast<long>(n); }
			Value & operator=(const char *s) { return *this = std::string(s); }

			bool operator==(const Value &) const;
			bool operator==(long) const;
			bool operator==(double) const;
			bool operator==(bool) const;
			bool operator==(const std::string &) const;
			bool operator==(int n) const { return *this == static_cast<long>(n); }
			bool operator==(const char *s) const { return *this == std::string(s); }

			explicit operator std::string() const;
			std::string escaped() const;
	};
}

std::ostream & operator<<(std::ostream &, const Spjalla::Config::Value &);

#endif
