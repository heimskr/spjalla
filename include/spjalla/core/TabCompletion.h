#ifndef SPJALLA_CORE_TAB_COMPLETION_H_
#define SPJALLA_CORE_TAB_COMPLETION_H_

#include <deque>
#include <string>
#include <vector>

#include "haunted/core/Key.h"
#include "spjalla/core/InputLine.h"

namespace Spjalla {
	class Client;
}

namespace Spjalla::Completions {
	/**
	 * Carries out the tab completion logic for a given input line. The raw string is passed as a reference that can
	 * be modified to add completion data. The cursor index is also passed too, and it should also be updated.
	 * For convenience, the index of the argument the cursor is in is provided, as well as the index within the
	 * argument. If either of them doesn't apply, they'll be negative. The function should return true if it handled
	 * updating the textinput's text and jumping to the cursor itself, or false if client::tab_complete should do it.
	 */
	using Completion_f = std::function<bool(Client &, const InputLine &, std::string &raw, size_t &cursor,
	                                        long arg_index, long sub)>;

	/** Completes the /set command. */
	bool completeSet(Client &, const InputLine &, std::string &raw, size_t &cursor, long arg_index, long sub);

	/** Completes nicknames but doesn't add ping suffixes. */
	bool completePlain(Client &, const InputLine &, std::string &raw, size_t &cursor, long arg_index, long sub);

	/** Contains the state data and logic for dealing with some parts of tab completion for commands. Clients keep an
	 *  instance of this and pass keypresses to it. */
	class CommandCompleter {
		private:
			Client *parent;

			/** When the user types a partial command and presses tab, it's stored here. If they press any key other
			 *  than tab, it's cleared. */
			std::string partial;

			bool hasPartial = false;

		public:
			CommandCompleter(Client &parent_): parent(&parent_) {}

			void onKey(const Haunted::Key &);
			void complete(std::string &, size_t &);
	};

	struct CompletionState {
		using Suggestor_f = std::function<std::string(const std::vector<std::string>)>;

		std::string partial;
		ssize_t partial_index = -1;
		ssize_t windex = -1;
		ssize_t sindex = -1;
		ssize_t cursor = -1;

		std::vector<Suggestor_f> suggestors {};

		CompletionState(const std::vector<Suggestor_f> &suggestors_ = {}): suggestors(suggestors_) {}

		/** Resets the partial string and partial index. */
		void reset();

		/** Returns whether nothing has been set (or if reset() has just been called). */
		bool empty() const;

		operator std::string() const;
	};
}

#endif
