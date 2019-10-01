#include <ostream>

#include "haunted/core/key.h"
#include "spjalla/core/client.h"
#include "spjalla/core/input_line.h"
#include "spjalla/core/util.h"
#include "spjalla/core/tab_completion.h"
#include "spjalla/config/defaults.h"

#include "lib/formicine/futil.h"

namespace spjalla::completions {
	void command_completer::on_key(const haunted::key &k) {
		// If tab is pressed, complete is called before on_key, so we don't handle filling in partial here.
		if (k != haunted::ktype::tab) {
			partial.clear();
			has_partial = false;
			for (auto &pair: parent.completion_states)
				pair.second.reset();
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

	void complete_set(client &client_, const input_line &line, std::string &raw, size_t &cursor, long arg_index,
	long arg_subindex) {
		completion_state &state = client_.completion_states["set"];
		// DBG("complete_set([" << line << "] [" << raw << "] [" << cursor << "] [" << arg_index << "] [" << arg_subindex << "])");
		if (2 <= arg_index)
			return;

		const std::string first_arg = line.args.empty()? "" : line.args[0];

		if (state.partial_index != arg_index) {
			state.partial = first_arg;
			state.partial_index = arg_index;
		}

		if (arg_index == 1) {
			const std::string rest = raw.substr(util::last_index_of_word(raw, 1));
			const std::string piece = first_arg.substr(0, arg_subindex);
			std::vector<std::string> keys = config::starts_with(state.partial);
			if (keys.empty())
				return;
			std::sort(keys.begin(), keys.end());
			std::string next = util::next_in_sequence(keys.begin(), keys.end(), piece);
			raw = "/set " + next + rest;
			cursor = next.length() + 5;
		} else if (arg_index == 2) {
			
		}
	}

	void completion_state::reset() {
		partial.clear();
		partial_index = -1;
	}

	completion_state::operator std::string() const {
		return "[" + std::to_string(partial_index) + "] \"" + partial + "\"";
	}

	std::ostream & operator<<(std::ostream &os, const completion_state &state) {
		return os << std::string(state);
	}
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
			DBG("[" << windex << ":" << sindex << "]");
			if (windex == 0) {
				// The user wants to complete a command name.
				completer.complete(text, cursor);
			} else {
				if (windex < 0) {
					// We're not in a word, but we're where one should be.
					windex = -windex - 1;
				}

				// The user has entered a command and the cursor is at or past the first argument.
				for (const auto &handler: command_handlers) {
					const std::string &name = handler.first;
					const commands::command &cmd = handler.second;

					if (name == il.command) {
						if (cmd.completion_fn)
							cmd.completion_fn(*this, il, text, cursor, windex, sindex);
						break;
					}
				}
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
