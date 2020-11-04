#include <deque>
#include <ostream>

#include "haunted/core/Key.h"

#include "pingpong/core/Channel.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/core/Util.h"
#include "spjalla/core/TabCompletion.h"
#include "spjalla/config/Defaults.h"

namespace Spjalla::Completions {
	void CommandCompleter::onKey(const Haunted::Key &k) {
		// If tab is pressed, complete is called before onKey, so we don't handle filling in partial here.
		if (k != Haunted::KeyType::Tab) {
			partial.clear();
			hasPartial = false;
			for (auto &pair: parent->completionStates)
				pair.second.reset();
		}
	}

	void CommandCompleter::complete(std::string &raw, size_t &cursor) {
		std::vector<std::string> split = formicine::util::split(raw, " ", true);
		std::string first = split[0].substr(1);

		if (!hasPartial) {
			partial = first;
			hasPartial = true;
		}

		std::vector<std::string> matches = parent->commandMatches(partial);

		// Don't bother doing anything if there are no matches.
		if (matches.size() == 0)
			return;

		const std::string rest = raw.substr(formicine::util::last_index_of_word(raw, 0));

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

	bool completeSet(Client &client_, const InputLine &line, std::string &raw, size_t &cursor, long arg_index,
	long arg_subindex) {
		CompletionState &state = client_.completionStates["set"];
		if (2 <= arg_index)
			return true;

		const std::string first_arg = line.args.empty()? "" : line.args[0];

		if (state.partial_index != arg_index) {
			state.partial = first_arg;
			state.partial_index = arg_index;
		}

		if (arg_index == 1) {
			const std::string rest = raw.substr(formicine::util::last_index_of_word(raw, 1));
			const std::string piece = first_arg.substr(0, arg_subindex);
			std::vector<std::string> keys = Config::startsWith(state.partial);
			if (keys.empty())
				return true;
			std::sort(keys.begin(), keys.end());
			std::string next = formicine::util::next_in_sequence(keys.begin(), keys.end(), piece);
			raw = "/set " + next + rest;
			cursor = next.length() + 5;
		}

		return false;
	}

	bool completePlain(Client &client_, const InputLine &, std::string &raw, size_t &cursor, long, long) {
		client_.completeMessage(raw, cursor, -1);
		return true;
	}

	void CompletionState::reset() {
		if (partial.empty())
			return;

		partial.clear();
		partial_index = -1;
		windex = -1;
		sindex = -1;
		cursor = -1;
	}

	bool CompletionState::empty() const {
		return partial_index == -1 && partial.empty() && windex == -1 && sindex == -1;
	}

	CompletionState::operator std::string() const {
		return "[" + std::to_string(partial_index) + "] \"" + partial + "\"";
	}

	std::ostream & operator<<(std::ostream &os, const CompletionState &state) {
		return os << std::string(state);
	}
}

namespace Spjalla {
	void Client::tabComplete() {
		std::string text = ui.input->getText();
		if (text.empty())
			return;

		size_t cursor = ui.input->getCursor();

		if (ui.activeWindow == ui.statusWindow && text.front() != '/') {
			text.insert(0, "/");
			ui.input->setText(text);
			ui.input->moveTo(++cursor);
			ui.input->jumpCursor();
		}

		InputLine il = getInputLine(text);
		ssize_t windex, sindex;
		std::tie(windex, sindex) = formicine::util::word_indices(text, cursor);

		if (il.isCommand()) {
			const std::string old_text {text};

			bool handled = false;

			if (windex == 0) {
				// The user wants to complete a command name.
				completer.complete(text, cursor);
			} else {
				if (windex < 0) {
					// We're not in a word, but we're where one should be.
					windex = -windex - 1;
				}

				// The user has entered a command and the cursor is at or past the first argument.

				// Search through each command handler.
				for (const auto &handler: commandHandlers) {
					const std::string &name = handler.first;
					const Commands::Command &cmd = handler.second;

					// If the command handler's name matches the command name...
					if (name == il.command) {
						// ...and the completion function is valid...
						if (cmd.completionFunction) {
							// ...then call the completion function.
							handled = cmd.completionFunction(*this, il, text, cursor, windex, sindex);
						}

						// Even if the completion function is invalid, stop the search.
						break;
					}
				}
			}

			if (!handled) {
				if (old_text != text)
					ui.input->setText(text);

				ui.input->moveTo(cursor);
				ui.input->jumpCursor();
			}
		} else if (activeServer()) {
			if (ui.activeWindow == ui.statusWindow && text.front() != '/') {
				text.insert(0, "/");
				ui.input->setText(text);
				ui.input->moveTo(++cursor);
				ui.input->jumpCursor();
			}

			completeMessage(text, cursor);
		}
	}

	void Client::completeMessage(std::string &text, size_t cursor, ssize_t word_offset) {
		if (text.empty())
			return;

		InputLine il = getInputLine(text);
		ssize_t windex, sindex;
		std::tie(windex, sindex) = formicine::util::word_indices(text, cursor);

		const std::string old_text {text};

		const UI::Window *window = ui.activeWindow;
		const PingPong::Server *server = window->server;
		const std::shared_ptr<PingPong::Channel> channel = window->channel;
		const std::shared_ptr<PingPong::User> user = window->user;

		Completions::CompletionState &state = completionStates["_"];
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

		const std::string &suffix = configs.get("completion", "ping_suffix").stringRef();
		formicine::util::remove_suffix(word, suffix);

		std::deque<std::string> items = {};
		bool do_sort = true; // For channels, the order is important and shouldn't be disturbed.

		const std::string lower = formicine::util::lower(state.partial);

		if (word[0] == '#') {
			for (const std::shared_ptr<PingPong::Channel> &ptr: server->channels) {
				if (formicine::util::lower(ptr->name).find(lower) == 0)
					items.push_back(ptr->name);
			}
		} else if (channel) {
			do_sort = false;
			for (const std::shared_ptr<PingPong::User> &ptr: channel->users) {
				if (formicine::util::lower(ptr->name).find(lower) == 0)
					items.push_back(ptr->name);
			}
		} else if (user) {
			items.push_back(user->name);
			items.push_back(server->getNick());
		} else {
			return;
		}

		if (!items.empty()) {
			if (do_sort)
				formicine::util::insensitive_sort(items.begin(), items.end());

			if (1 < items.size()) {
				// It's reasonable to assume that you're not desparate to complete your own nick, so for your
				// convenience we'll move your name to the end of the list.
				const std::string self = server->getNick();
				for (auto iter = items.begin(); iter != items.end() && iter + 1 != items.end(); ++iter) {
					if (*iter == self) {
						items.erase(iter);
						items.push_back(self);
						break;
					}
				}
			}

			const std::string next = formicine::util::next_in_sequence(items.begin(), items.end(), word);
			cursor = formicine::util::replace_word(text, windex, next);

			if (windex == word_offset && next.front() != '#' && !suffix.empty()) {
				// Erase any space already after the colon to prevent spaces from accumulating.
				if (cursor < text.length() && std::isspace(text[cursor]))
					text.erase(cursor, 1);

				text.insert(cursor, suffix + " ");
				cursor += suffix.length() + 1;
			}
		}

		if (old_text != text)
			ui.input->setText(text);

		ui.input->moveTo(cursor);
		ui.input->jumpCursor();
	}

	void Client::keyPostlistener(const Haunted::Key &k) {
		completer.onKey(k);
	}
}
