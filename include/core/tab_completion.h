#ifndef SPJALLA_CORE_TAB_COMPLETION_H_
#define SPJALLA_CORE_TAB_COMPLETION_H_

#include <string>

#include "core/input_line.h"

namespace spjalla {
	/**
	 * Carries out the tab completion logic for a given input line. The raw string is passed as a reference that can
	 * be modified to add completion data. The cursor index is also passed too, and it should also be updated.
	 * For convenience, the index of the argument the cursor is in is provided, as well as the index within the
	 * argument. If either of them doesn't apply, they'll be negative.
	 */
	using completer =
		std::function<void(const input_line &, std::string &raw, size_t &index, long arg_index, long arg_subindex)>;

	namespace completions {
		void set_complete(const input_line &, std::string &raw, size_t &index, long arg_index, long arg_subindex);
	}
}

#endif
