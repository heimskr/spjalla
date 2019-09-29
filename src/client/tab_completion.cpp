#include "core/client.h"
#include "core/input_line.h"
#include "core/sputil.h"
#include "core/tab_completion.h"

#include "lib/formicine/futil.h"

namespace spjalla::completions {
	void complete_command(client &client_, const input_line &line, std::string &raw, size_t &cursor, long arg_index,
	long) {
		if (arg_index != 0)
			return;

		std::vector<std::string> split = formicine::util::split(raw, " ", true);
		std::string &first = split[0];
		std::vector<std::string> matches = client_.command_matches(first.substr(1));

		// Don't bother doing anything if there are no matches.
		if (matches.size() == 0)
			return;

		if (matches.size() == 1) {
			const std::string rest = raw.substr(util::last_index_of_word(raw, 0));
			raw = "/" + matches[0] + rest;
			cursor = matches[0].length() + 1;

			if (cursor == raw.length())
				raw.push_back(' ');

			++cursor;
			return;
		}

		for (std::string &match: matches)
			match.insert(0, "/");

		client_.log("Matches: " + util::join(matches.begin(), matches.end(), " "));
	}

	// void complete_set(const input_line &line, std::string &raw, size_t &index, long arg_index, long arg_subindex) {

	// }
}

namespace spjalla {
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

		const std::string old_text {text};
		const size_t old_cursor = cursor;

		if (il.is_command()) {
			if (windex == 0) {
				// The user wants to complete a command name.
				completions::complete_command(*this, il, text, cursor, windex, sindex);
			} else if (0 < windex) {
				// The user has entered a command and the cursor is at or past the first argument.
			}
		}

		if (old_text != text)
			ui.input->set_text(text);

		if (old_cursor != cursor) {
			ui.input->move_to(cursor);
			ui.input->jump_cursor();
		}
	}
}
