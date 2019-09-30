#ifndef SPJALLA_CONFIG_VALUE_H_
#define SPJALLA_CONFIG_VALUE_H_

#include <string>

namespace spjalla::config {
	enum class value_type {invalid, long_, double_, string_};

	class value {
		// Boost? Never heard of it.
		private:
			value_type type;
			long long_value {0};
			double double_value {0.};
			std::string string_value {};

		public:
			value(long long_):     type(value_type::long_),   long_value(long_) {}
			value(double double_): type(value_type::double_), double_value(double_) {}
			value(const std::string &str_): type(value_type::string_), string_value(str_) {}
			value(int int_):         value(static_cast<long>(int_)) {}
			value(const char *str_): value(std::string(str_)) {}

			value_type get_type() const { return type; }
			long & long_ref();
			double & double_ref();
			std::string & string_ref();

			long long_() const;
			double double_() const;
			const std::string & string_() const;

			bool is_long()   const { return type == value_type::long_;   }
			bool is_double() const { return type == value_type::double_; }
			bool is_string() const { return type == value_type::string_; }

			value & operator=(long);
			value & operator=(double);
			value & operator=(const std::string &);
			value & operator=(int n) { return *this = static_cast<long>(n); }
			value & operator=(const char *s) { return *this = std::string(s); }

			bool operator==(const value &) const;
			bool operator==(long) const;
			bool operator==(double) const;
			bool operator==(const std::string &) const;
			bool operator==(int n) const { return *this == static_cast<long>(n); }
			bool operator==(const char *s) const { return *this == std::string(s); }

			explicit operator std::string() const;
			std::string escaped() const;
	};
}

#endif
