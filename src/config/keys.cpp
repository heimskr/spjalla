#include "spjalla/config/keys.h"

namespace spjalla::config {
	Haunted::key keys::toggle_overlay  = {Haunted::ktype::semicolon, Haunted::kmod::ctrl};
	Haunted::key keys::switch_server   = {Haunted::ktype::x,         Haunted::kmod::ctrl};
	Haunted::key keys::next_window     = {Haunted::ktype::n,         Haunted::kmod::ctrl};
	Haunted::key keys::previous_window = {Haunted::ktype::p,         Haunted::kmod::ctrl};
	Haunted::key keys::toggle_mouse    = {Haunted::ktype::m,         Haunted::kmod::ctrl};
}