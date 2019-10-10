#include "spjalla/core/client.h"
#include "spjalla/plugins/logs.h"
#include "spjalla/plugins/logs/log_line.h"

namespace spjalla::plugins::logs {
	void logs_plugin::clean() {
		ui::interface &ui = parent->get_ui();
		
		int rows_removed = 0, total_rows = 0;
		ui::window *window = ui.get_active_window();
		const int width = window->get_position().width;
		std::deque<std::unique_ptr<haunted::ui::textline>> &lines = window->get_lines(), new_lines = {};

		for (std::unique_ptr<haunted::ui::textline> &ptr: lines) {
			int rows = ptr->num_rows(width);
			if (log_line *ll = dynamic_cast<log_line *>(ptr.get())) {
				if (total_rows < window->get_voffset())
					rows_removed += rows;
			} else {
				new_lines.push_back(std::move(ptr));
			}

			total_rows += rows;
		}

		lines.swap(new_lines);
		window->vscroll(-rows_removed);
		window->draw();
	}
}
