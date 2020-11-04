#include "haunted/core/Key.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/core/PluginHost.h"

#include "spjalla/plugins/Plugin.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Plugins {
	struct HistoryPlugin: public Plugin {
		~HistoryPlugin() {}

		std::string getName()        const override { return "History"; }
		std::string getDescription() const override { return "Lets you repeat old inputs with the up and down keys."; }
		std::string getVersion()     const override { return "0.1.0"; }

		int commandIndex = 0;
		size_t maxLength = 4096;
		std::deque<std::string> history {};

		std::shared_ptr<PluginHost::Pre_f<Haunted::Key>> prekey = 
			std::make_shared<PluginHost::Pre_f<Haunted::Key>>([this](const Haunted::Key &key, bool) {
				if (!history.empty()) {
					if (key == Haunted::KeyType::UpArrow && 0 < command_index) {
						parent->getUI().set_input(history[--command_index]);
						return cancelable_result::disable;
					}

					if (key == Haunted::KeyType::down_arrow && command_index < static_cast<int>(history.size()) - 1) {
						parent->getUI().set_input(history[++command_index]);
						return cancelable_result::disable;
					}
				}

				return cancelable_result::pass;
			});

		std::shared_ptr<PluginHost::post_function<InputLine>> postinput =
			std::make_shared<PluginHost::post_function<InputLine>>([this](const InputLine &il) {
				history.push_back(il.original);

				if (max_length < history.size())
					history.pop_front();

				command_index = history.size();
			});

		void postinit(PluginHost *host) override {
			parent = dynamic_cast<Spjalla::Client *>(host);
			if (!parent) {
				DBG("Error: expected client as plugin host");
				return;
			}

			parent->handle(prekey);
			parent->handle(postinput);
		}

		void cleanup(PluginHost *) override {
			parent->unhandle(prekey);
			parent->unhandle(postinput);
		}
	};
}

spjalla::plugins::HistoryPlugin ext_plugin {};
