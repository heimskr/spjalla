#ifndef LIB_UTF8_H_
#define LIB_UTF8_H_

#include <string>

#include "unicode/unistr.h"

namespace spjalla {
	using utf8char = int32_t;


	class utf8 {
		public:
			static size_t width(unsigned char);
	};

	class utf8str {
		private:
			icu::UnicodeString unistr;

		public:
			utf8str() {}
			utf8str(const std::string &str): unistr(icu::UnicodeString::fromUTF8(str)) {}
			utf8str(char ch): utf8str(std::string(1, ch)) {}

			utf8char operator[](ssize_t) const;
			utf8str & operator=(const utf8str &);
			utf8str & operator=(const icu::UnicodeString &);
			utf8str & operator=(utf8str &&);
			utf8str & operator=(icu::UnicodeString &&);
			bool operator==(const utf8str &) const;
			operator std::string() const;

			std::string substr(ssize_t, ssize_t) const;
			std::string substr(ssize_t) const;
			void insert(ssize_t, const std::string &);
			void insert(ssize_t, char);
			void erase(ssize_t, ssize_t);
			void erase(ssize_t);
			size_t size()   const;
			size_t length() const;
			bool   empty()  const;
			void   clear();

			friend std::ostream & operator<<(std::ostream &os, const utf8str &input);
	};
}

#endif
