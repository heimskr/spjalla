#ifndef SPJALLA_CORE_TAB_COMPLETION_H_
#define SPJALLA_CORE_TAB_COMPLETION_H_

#include <string>

#include "core/input_line.h"

namespace spjalla {
	class client;

	/**
	 * Carries out the tab completion logic for a given input line. The raw string is passed as a reference that can
	 * be modified to add completion data. The cursor index is also passed too, and it should also be updated.
	 * For convenience, the index of the argument the cursor is in is provided, as well as the index within the
	 * argument. If either of them doesn't apply, they'll be negative.
	 */
	using completer =
		std::function<void(client &, const input_line &, std::string &raw, size_t &cursor, long arg_index, long sub)>;

	namespace completions {
		void complete_command(client &, const input_line &, std::string &raw, size_t &cursor, long arg_index, long sub);

		/** Completes the /set command. */
		void complete_set(client &, const input_line &, std::string &raw, size_t &cursor, long arg_index, long sub);
	}
}

#endif
