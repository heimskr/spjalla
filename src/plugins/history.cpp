#include "core/client.h"
#include "core/plugin_host.h"
#include "plugins/plugin.h"

#include "formicine/ansi.h"

struct history_plugin: spjalla::plugins::plugin {
	virtual ~history_plugin() {}

	std::string get_name() const override { return "History"; }
	std::string get_version() const override { return "0.0.0"; }

	void startup(spjalla::plugins::plugin_host *host) override {
		spjalla::client *client = dynamic_cast<spjalla::client *>(host);
		if (!client) {
			DBG("Error: expected client as plugin host");
			return;
		}


		DBG("history::startup()");
	}
};

history_plugin ext_plugin {};
