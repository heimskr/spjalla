#ifndef SPJALLA_CONFIG_KEYS_H_
#define SPJALLA_CONFIG_KEYS_H_

#include "haunted/core/Key.h"

namespace Spjalla::Config {
	struct Keys {
		static Haunted::Key toggleOverlay, switchServer, nextWindow, previousWindow, toggleMouse;
	};
}

#endif
