#include "lib/haunted/core/key.h"
#include "core/client.h"
#include "core/input_line.h"
#include "core/sputil.h"
#include "core/tab_completion.h"

#include "lib/formicine/futil.h"

namespace spjalla::completions {
	void command_completer::on_key(const haunted::key &k) {
		// If tab is pressed, complete is called before on_key, so we don't handle filling in partial here.
		if (k != haunted::ktype::tab) {
			partial.clear();
			has_partial = false;
		}
	}

	void command_completer::complete(std::string &raw, size_t &cursor) {
		std::vector<std::string> split = formicine::util::split(raw, " ", true);
		std::string first = split[0].substr(1);

		if (!has_partial) {
			partial = first;
			has_partial = true;
		}

		std::vector<std::string> matches = parent.command_matches(partial);

		// Don't bother doing anything if there are no matches.
		if (matches.size() == 0)
			return;

		const std::string rest = raw.substr(util::last_index_of_word(raw, 0));

		if (matches.size() == 1) {
			raw = "/" + matches[0] + rest;
			cursor = matches[0].length() + 1;

			if (cursor == raw.length())
				raw.push_back(' ');

			++cursor;
			return;
		}


		for (auto iter = matches.begin(), end = matches.end(); iter != end; ++iter) {
			if (*iter == first) {
				// The user has pressed tab again after one of the completions has been filled in. Fill in the next
				// match.
				auto next = iter == end - 1? matches.begin() : iter + 1;
				raw = "/" + *next + rest;
				cursor = next->length() + 1;
				return;
			}
		}

		// There are multiple matches but none has been filled in yet. Start with the first.
		raw = "/" + matches[0] + rest;
		cursor = matches[0].length() + 1;
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
				completer.complete(text, cursor);
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

	void client::key_postlistener(const haunted::key &k) {
		completer.on_key(k);
	}
}
