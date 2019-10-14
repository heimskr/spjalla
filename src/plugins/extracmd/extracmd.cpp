#include "spjalla/core/client.h"
#include "spjalla/core/plugin_host.h"

#include "spjalla/lines/raw.h"

#include "spjalla/plugins/plugin.h"

#include "lib/formicine/ansi.h"

namespace spjalla::plugins {
	struct extracmd_plugin: public plugin {
		~extracmd_plugin() {}

		std::string get_name()        const override { return "Extra Commands"; }
		std::string get_description() const override { return "Adds a few extra utility commands."; }
		std::string get_version()     const override { return "0.0.1"; }

		void postinit(plugin_host *host) override {
			spjalla::client *client = dynamic_cast<spjalla::client *>(host);
			if (!client) { DBG("Error: expected client as plugin host"); return; }

			client->add({"rmraw", {0, 0, false, [client](pingpong::server *, const input_line &) {
				client->get_ui().get_active_window()->remove_rows([](const haunted::ui::textline *line) -> bool {
					return dynamic_cast<const lines::raw_line *>(line);
				});
			}, {}}});

			client->add({"_foo", {0, 0, false, [client](pingpong::server *, const input_line &) {
				ui::window *win = client->get_ui().get_active_window();
				for (int i = 0; i < win->get_position().height - 2; ++i)
					*win += lines::raw_line(client, "Foo " + std::to_string(i));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				DBG("Go.");
				*win += lines::raw_line(client, std::string(405, '.'));
			}, {}}});

			client->add({"_bar", {0, 0, false, [client](pingpong::server *, const input_line &) {
				*client->get_ui().get_active_window() += lines::raw_line(client, std::string(405, '.'));
			}, {}}});

			client->add({"_asc", {0, 0, false, [client](pingpong::server *, const input_line &) {
				ui::window *win = client->get_ui().get_active_window();
				win->set_autoscroll(!win->get_autoscroll());
				DBG("Autoscroll is now " << ansi::bold(win->get_autoscroll()? "on" : "off") << ".");
			}, {}}});
		}
	};
}

spjalla::plugins::extracmd_plugin ext_plugin {};
