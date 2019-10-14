#include "spjalla/core/client.h"
#include "spjalla/plugins/logs.h"
#include "spjalla/plugins/logs/log_line.h"

namespace spjalla::plugins::logs {
	void logs_plugin::clean() {
		parent->get_ui().get_active_window()->remove_rows([&](const haunted::ui::textline *line) -> bool {
			return dynamic_cast<const log_line *>(line);
		});
	}
}
