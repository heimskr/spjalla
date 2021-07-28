#include "spjalla/core/Client.h"
#include "spjalla/plugins/Plugin.h"

namespace Spjalla::Plugins {
	bool Plugin::unload() {
		if (PluginHost::PluginTuple *tuple = parent->getPlugin(this)) {
			parent->unloadPlugin(*tuple);
			return true;
		}

		return false;
	}
}
