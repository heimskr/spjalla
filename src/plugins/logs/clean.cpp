#include "spjalla/core/Client.h"
#include "spjalla/plugins/Logs.h"
#include "spjalla/plugins/logs/log_line.h"

namespace Spjalla::Plugins::logs {
	void logs_plugin::clean() {
		parent->getUI().get_active_window()->remove_rows([&](const Haunted::UI::textline *line) -> bool {
			return dynamic_cast<const log_line *>(line);
		});
	}
}
