#include "core/client.h"
#include "plugins/plugin.h"

#include "formicine/ansi.h"

struct history_plugin: spjalla::plugins::plugin {
	virtual ~history_plugin() {}

	std::string get_name() const override { return "History"; }
	std::string get_version() const override { return "0.0.0"; }

	void startup(spjalla::client &client) override {
		DBG("history::startup()");
	}
};

history_plugin ext_plugin {};
