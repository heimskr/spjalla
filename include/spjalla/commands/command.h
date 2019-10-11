#ifndef SPJALLA_COMMANDS_COMMANDS_H_
#define SPJALLA_COMMANDS_COMMANDS_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "spjalla/core/tab_completion.h"

namespace pingpong {
	class server;
}

namespace spjalla {
	class client;
}

namespace spjalla::ui {
	class interface;
}

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

	void do_disconnect(client &, pingpong::server *, const input_line &);
	void do_me(ui::interface &, const input_line &);
	void do_move(ui::interface &, const input_line &);
	void do_mode(ui::interface &, pingpong::server *, const input_line &);
	void do_part(client &, pingpong::server *, const input_line &);
	void do_set(client &, const input_line &);
	void do_spam(ui::interface &, const input_line &);
	void do_topic(client &, pingpong::server *, const input_line &);
}

#endif
