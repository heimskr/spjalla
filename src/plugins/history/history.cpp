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
					if (key == Haunted::KeyType::UpArrow && 0 < commandIndex) {
						parent->getUI().setInput(history[--commandIndex]);
						return CancelableResult::Disable;
					}

					if (key == Haunted::KeyType::DownArrow && commandIndex < static_cast<int>(history.size()) - 1) {
						parent->getUI().setInput(history[++commandIndex]);
						return CancelableResult::Disable;
					}
				}

				return CancelableResult::Pass;
			});

		std::shared_ptr<PluginHost::Post_f<InputLine>> postinput =
			std::make_shared<PluginHost::Post_f<InputLine>>([this](const InputLine &il) {
				history.push_back(il.original);

				if (maxLength < history.size())
					history.pop_front();

				commandIndex = history.size();
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

Spjalla::Plugins::HistoryPlugin ext_plugin {};
