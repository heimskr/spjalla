#include <unordered_map>

#include "spjalla/plugins/Coloring.h"

namespace Spjalla::Plugins {
	bool ColoringPlugin::onInterrupt() {
		parent->getUI().input->insert("\x03");
		parent->getUI().input->drawInsert();
		return false;
	}

	void ColoringPlugin::postinit(PluginHost *host) {
		parent = dynamic_cast<Spjalla::Client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }
		parent->getTerminal().onInterrupt = [this]() { return ColoringPlugin::onInterrupt(); };
		using String = Haunted::UI::TextInput::String;
		render = [&](const String &str) -> String {
			if (str == "\x02") return ansi::wrap("B", ansi::style::inverse);
			if (str == "\x03") return ansi::wrap("C", ansi::style::inverse);
			return str;
		};
		parent->getUI().input->characterRenderers.emplace("p:Coloring", render);
		parent->handle(prekey);
	}

	void ColoringPlugin::cleanup(PluginHost *) {
		parent->getTerminal().onInterrupt = {};
		parent->getUI().input->characterRenderers.erase("p:Coloring");
		parent->unhandle(prekey);
	}
}

Spjalla::Plugins::ColoringPlugin ext_plugin {};
