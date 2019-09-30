#ifndef SPJALLA_COMMANDS_COMMANDS_H_
#define SPJALLA_COMMANDS_COMMANDS_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "pingpong/core/server.h"

#include "spjalla/core/tab_completion.h"

namespace spjalla::commands {
	struct command {
		using handler_fn = std::function<void(pingpong::server *, const input_line &)>;

		int min_args, max_args;
		bool needs_server;
		handler_fn handler;
		completions::completion_fn completion_fn;
		std::vector<completions::completion_state::suggestor_fn> suggestors;

		command() = delete;

		command(int min_args_, int max_args_, bool needs_server_, const handler_fn &handler_,
		const completions::completion_fn &completion_fn_ = {},
		const std::vector<completions::completion_state::suggestor_fn> &suggestors_= {}):
			min_args(min_args_), max_args(max_args_), needs_server(needs_server_), handler(handler_),
			completion_fn(completion_fn_), suggestors(suggestors_) {}

	};

	/** Command name, command */
	using pair = std::pair<std::string, command>;
}

#endif
