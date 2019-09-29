#include "core/client.h"
#include "core/tab_completion.h"

namespace spjalla::completions {
	void complete_set(const input_line &line, std::string &raw, size_t &index, long arg_index, long arg_subindex) {

	}
}

namespace {
	void client::tab_complete() {
		std::string text = ui.input->get_text();
		if (text.empty())
			return;

		size_t cursor = ui.input->get_cursor();

		if (ui.active_window == ui.status_window && text.front() != '/') {
			text.insert(0, "/");
			ui.input->set_text(text);
			ui.input->move_to(++cursor);
			ui.input->jump_cursor();
		}

		input_line il = get_input_line(text);
		ssize_t windex, sindex;
		std::tie(windex, sindex) = util::word_indices(text, cursor);
		DBG("Command[" << il.command << "], windex[" << windex << ":" << sindex << "]");

		if (il.is_command() && 0 < windex) {
			// The user has entered a command and the cursor is at or past the first argument.
		}
	}
}
