#include "lines/userlist.h"

namespace spjalla::lines {
	userlist_line::operator std::string() const {
		return user->name;
	}
}
