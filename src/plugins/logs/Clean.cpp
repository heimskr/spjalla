#include "spjalla/core/Client.h"
#include "spjalla/plugins/Logs.h"
#include "spjalla/plugins/logs/LogLine.h"

namespace Spjalla::Plugins::Logs {
	void LogsPlugin::clean() {
		parent->getUI().getActiveWindow()->removeRows([&](const Haunted::UI::TextLine *line) -> bool {
			return dynamic_cast<const LogLine *>(line);
		});
	}
}
