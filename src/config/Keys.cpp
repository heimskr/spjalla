#include "spjalla/config/Keys.h"

namespace Spjalla::Config {
	Haunted::Key Keys::toggleOverlay  = {Haunted::KeyType::Semicolon, Haunted::KeyMod::Ctrl};
	Haunted::Key Keys::switchServer   = {Haunted::KeyType::x,         Haunted::KeyMod::Ctrl};
	Haunted::Key Keys::nextWindow     = {Haunted::KeyType::n,         Haunted::KeyMod::Ctrl};
	Haunted::Key Keys::previousWindow = {Haunted::KeyType::p,         Haunted::KeyMod::Ctrl};
	Haunted::Key Keys::toggleMouse    = {Haunted::KeyType::m,         Haunted::KeyMod::Ctrl};
}