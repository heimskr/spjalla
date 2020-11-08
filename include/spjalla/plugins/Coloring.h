#ifndef SPJALLA_PLUGINS_COLORING_H_
#define SPJALLA_PLUGINS_COLORING_H_

#include "spjalla/plugins/Plugin.h"
#include "spjalla/core/Client.h"
#include "spjalla/core/PluginHost.h"
#include "haunted/ui/TextInput.h"

namespace Spjalla::Plugins {
	class ColoringPlugin: public Plugin {
		private:
			bool onInterrupt();
			Haunted::UI::TextInput::RenderChar_f render;

		public:
			virtual ~ColoringPlugin() {}

			std::string getName()        const override { return "Coloring"; }
			std::string getDescription() const override { return "Provides the ability to input colors."; }
			std::string getVersion()     const override { return "0.0.1"; }

			void postinit(PluginHost *) override;
			void cleanup(PluginHost  *) override;

			std::shared_ptr<PluginHost::Pre_f<Haunted::Key>> prekey =
				std::make_shared<PluginHost::Pre_f<Haunted::Key>>([this](const Haunted::Key &key, bool) {
					if (key.isCtrl(Haunted::KeyType::b)) {
						parent->getUI().input->insert("\x02");
						parent->getUI().input->drawInsert();
						return CancelableResult::Disable;
					}

					return CancelableResult::Pass;
				});
	};
}

#endif
