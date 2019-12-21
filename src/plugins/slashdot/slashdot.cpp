#include <cpr/cpr.h>
#include <thread>

#include "spjalla/core/client.h"
#include "spjalla/plugins/slashdot.h"
#include "spjalla/ui/basic_window.h"
#include "spjalla/lines/basic.h"

namespace spjalla::plugins {
	void slashdot_plugin::postinit(plugin_host *host) {
		parent = dynamic_cast<spjalla::client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		parent->add(".", 0, 0, false, [this](pingpong::server *, const input_line &) {
			std::thread thread([this]() {
				DBG("Retrieving slashdot.xml...");
				ui::window *win = parent->get_ui().new_window<ui::basic_window>("slashdot");
				parent->get_ui().focus_window(win);
				*win += lines::basic_line(parent, "Fetching story metadata...");
				auto res = cpr::Get(cpr::Url("https://slashdot.org/slashdot.xml"));
				win->clear_lines();
				if (res.status_code != 200) {
					*win += lines::basic_line(parent, "Couldn't fetch stories (status: " +
						std::to_string(res.status_code) + ")");
					return;
				}

				std::string raw = res.text;
				slashdot::parser slash;
				slash.parse(raw);
				if (slash.stories.empty()) {
					*win += lines::basic_line(parent, "No stories were found.");
					return;
				}


				for (const slashdot::story &story: slash.stories) {
					*win += ansi::bold(story.title) + " (posted under " + ansi::italic(story.section) + " on " +
					        story.time + " by " + story.author + " from the " + ansi::italic(story.department) +
					        " dept.)";
					*win += "";
					*win += ansi::dim("...");
					*win += "";
					*win += "";
				}

				win->set_voffset(0);
				slash.fetch();

				win->clear_lines();

				for (const slashdot::story &story: slash.stories) {
					*win += ansi::bold(story.title) + " (posted under " + ansi::italic(story.section) + " on " +
					        story.time + " by " + story.author + " from the " + ansi::italic(story.department) +
					        " dept.)";
					*win += "";
					for (const std::string &str: formicine::util::split(story.text, "\n", false))
						*win += lines::basic_line(parent, "    " + str, 4);
					*win += "";
					*win += "";
				}

				win->set_voffset(0);
				if (parent->get_ui().get_active_window() == win)
					win->draw();
			});

			thread.detach();
		});
	}

	void slashdot_plugin::cleanup(plugin_host *) {
		parent->remove_command(".");
	}
}

spjalla::plugins::slashdot_plugin ext_plugin {};
