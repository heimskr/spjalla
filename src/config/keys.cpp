#include "spjalla/config/keys.h"

namespace spjalla::config {
	haunted::key keys::toggle_overlay  = {haunted::ktype::semicolon, haunted::kmod::ctrl};
	haunted::key keys::switch_server   = {haunted::ktype::x,         haunted::kmod::ctrl};
	haunted::key keys::next_window     = {haunted::ktype::n,         haunted::kmod::ctrl};
	haunted::key keys::previous_window = {haunted::ktype::p,         haunted::kmod::ctrl};
	haunted::key keys::toggle_mouse    = {haunted::ktype::m,         haunted::kmod::ctrl};
}