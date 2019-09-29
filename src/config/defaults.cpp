#include "pingpong/core/irc.h"

#include "config/config.h"

namespace spjalla::config {
	void database::register_defaults() {
		register_key("server", "default_nick", pingpong::irc::default_nick);
		register_key("server", "default_user", pingpong::irc::default_user);
		register_key("server", "default_real", pingpong::irc::default_realname);
	}
}
