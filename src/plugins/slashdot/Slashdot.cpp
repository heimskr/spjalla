#include "spjalla/core/Client.h"
#include "spjalla/plugins/Slashdot.h"
#include "spjalla/ui/BasicWindow.h"
#include "spjalla/lines/Basic.h"

#ifdef ENABLE_SLASHDOT
#include <cpr/cpr.h>
#include <thread>

namespace Spjalla::Plugins {
	void SlashdotPlugin::postinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		parent->add(".", 0, 0, false, [this](PingPong::Server *, const InputLine &) {
			std::thread thread([this]() {
				DBG("Retrieving slashdot.xml...");
				UI::Window *win = parent->getUI().newWindow<UI::BasicWindow>("slashdot");
				parent->getUI().focusWindow(win);
				*win += Lines::BasicLine(parent, "Fetching story metadata...");
				auto res = cpr::Get(cpr::Url("https://slashdot.org/slashdot.xml"));
				win->clearLines();
				if (res.status_code != 200) {
					*win += Lines::BasicLine(parent, "Couldn't fetch stories (status: " +
						std::to_string(res.status_code) + ")");
					return;
				}

				std::string raw = res.text;
				Slashdot::Parser slash;
				slash.parse(raw);
				if (slash.stories.empty()) {
					*win += Lines::BasicLine(parent, "No stories were found.");
					return;
				}


				for (const Slashdot::Story &story: slash.stories) {
					*win += ansi::bold(story.title) + " (posted under " + ansi::italic(story.section) + " on " +
					        story.time + " by " + story.author + " from the " + ansi::italic(story.department) +
					        " dept.)";
					*win += "";
					*win += ansi::dim("...");
					*win += "";
					*win += "";
				}

				win->setVoffset(0);
				slash.fetch();

				win->clearLines();

				for (const Slashdot::Story &story: slash.stories) {
					*win += ansi::bold(story.title) + " (posted under " + ansi::italic(story.section) + " on " +
					        story.time + " by " + story.author + " from the " + ansi::italic(story.department) +
					        " dept.)";
					*win += "";
					for (const std::string &str: formicine::util::split(story.text, "\n", false))
						*win += Lines::BasicLine(parent, "    " + str, 4);
					*win += "";
					*win += "";
				}

				win->setVoffset(0);
				if (parent->getUI().getActiveWindow() == win)
					win->draw();
			});

			thread.detach();
		});
	}

	void SlashdotPlugin::cleanup(PluginHost *) {
		parent->removeCommand(".");
	}
}
#endif

Spjalla::Plugins::SlashdotPlugin ext_plugin {};
