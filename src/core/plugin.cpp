#include "spjalla/core/client.h"
#include "spjalla/plugins/plugin.h"

namespace spjalla::plugins {
	bool plugin::unload() {
		if (plugin_host::plugin_tuple *tuple = parent->get_plugin(this)) {
			parent->unload_plugin(*tuple);
			return true;
		}

		return false;
	}
}
