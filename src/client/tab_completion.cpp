#include <deque>
#include <ostream>

#include "haunted/core/key.h"

#include "pingpong/core/channel.h"

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
				// The user has pressed tab again after one of the completions has been filled in. Fill in the next one.
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
		}
	}

	void complete_me(client &client_, const input_line &, std::string &raw, size_t &cursor, long, long) {
		client_.complete_message(raw, cursor, 1);
	}

	void completion_state::reset() {
		if (partial.empty())
			return;

		partial.clear();
		partial_index = -1;
		windex = -1;
		sindex = -1;
		cursor = -1;
	}

	bool completion_state::empty() const {
		return partial_index == -1 && partial.empty() && windex == -1 && sindex == -1;
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


		if (il.is_command()) {
			DBG("hehe...");
			const std::string old_text {text};

			if (windex == 0) {
				// The user wants to complete a command name.
				completer.complete(text, cursor);

				if (old_text != text)
					ui.input->set_text(text);

				ui.input->move_to(cursor);
				ui.input->jump_cursor();
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
		} else if (ui.get_active_server()) {
			if (ui.active_window == ui.status_window && text.front() != '/') {
				text.insert(0, "/");
				ui.input->set_text(text);
				ui.input->move_to(++cursor);
				ui.input->jump_cursor();
			}

			complete_message(text, cursor);
		}
	}

	void client::complete_message(std::string &text, size_t cursor, ssize_t word_offset) {
		if (text.empty())
			return;

		input_line il = get_input_line(text);
		ssize_t windex, sindex;
		std::tie(windex, sindex) = util::word_indices(text, cursor);

		const std::string old_text {text};

		const ui::window *window = ui.active_window;
		const pingpong::server *server = window->data.serv;
		const std::shared_ptr<pingpong::channel> channel = window->data.chan;
		const std::shared_ptr<pingpong::user> user = window->data.user;

		completions::completion_state &state = completion_states["_"];
		std::string word;
		if (!state.empty()) {
			windex = state.windex;
			sindex = state.sindex;
			word = formicine::util::nth_word(text, windex);
		} else {
			word = formicine::util::nth_word(text, windex);
			if (!word.empty()) {
				state.windex = windex;
				state.sindex = sindex;
				state.partial = word;
			} else {
				return;
			}
		}

		const std::string &suffix = configs.get("completion", "ping_suffix").string_ref();
		util::remove_suffix(word, suffix);

		std::deque<std::string> items = {};

		if (word[0] == '#') {
			for (const std::shared_ptr<pingpong::channel> &ptr: server->channels) {
				if (ptr->name.find(state.partial) == 0)
					items.push_back(ptr->name);
			}
		} else if (channel) {
			for (const std::shared_ptr<pingpong::user> &ptr: channel->users) {
				if (ptr->name.find(state.partial) == 0)
					items.push_back(ptr->name);
			}
		} else if (user) {
			items.push_back(user->name);
			items.push_back(server->get_nick());
		} else {
			return;
		}

		if (!items.empty()) {
			std::sort(items.begin(), items.end());

			if (1 < items.size()) {
				// It's reasonable to assume that you're not desparate to complete your own nick, so for your
				// convenience we'll move your name to the end of the list.
				const std::string self = server->get_nick();
				for (auto iter = items.begin(); iter != items.end() && iter + 1 != items.end(); ++iter) {
					if (*iter == self) {
						items.erase(iter);
						items.push_back(self);
						break;
					}
				}
			}

			const std::string next = util::next_in_sequence(items.begin(), items.end(), word);
			cursor = util::replace_word(text, windex, next);

			if (windex == word_offset && next.front() != '#' && !suffix.empty()) {
				// Erase any space already after the colon to prevent spaces from accumulating.
				if (cursor < text.length() && std::isspace(text[cursor]))
					text.erase(cursor, 1);

				text.insert(cursor, suffix + " ");
				cursor += suffix.length() + 1;
			}
		}

		if (old_text != text)
			ui.input->set_text(text);

		ui.input->move_to(cursor);
		ui.input->jump_cursor();
	}

	void client::key_postlistener(const haunted::key &k) {
		completer.on_key(k);
	}
}
