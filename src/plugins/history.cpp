#include "plugins/plugin.h"

struct history_plugin: spjalla::plugins::plugin {
	virtual ~history_plugin() {}

	std::string get_name() const override { return "History"; }
	std::string get_version() const override { return "0.0.0"; }
};

history_plugin ext_plugin {};
