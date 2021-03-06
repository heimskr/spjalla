#ifndef SPJALLA_COMMANDS_COMMANDS_H_
#define SPJALLA_COMMANDS_COMMANDS_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "spjalla/core/TabCompletion.h"

namespace PingPong {
	class Server;
}

namespace Spjalla {
	class Client;
}

namespace Spjalla::UI {
	class Interface;
}

namespace Spjalla::Commands {
	struct Command {
		// If the function returns true, Client::handleLine will stop iterating through the command handlers multimap.
		using BoolHandler_f = std::function<bool(PingPong::Server *, const InputLine &)>;
		using     Handler_f = std::function<void(PingPong::Server *, const InputLine &)>;

		int minArgs, maxArgs;
		bool needsServer;
		BoolHandler_f handler;
		Completions::Completion_f completionFunction;
		std::vector<Completions::CompletionState::Suggestor_f> suggestors;

		Command() = delete;

		Command(int min_args, int max_args, bool needs_server, const BoolHandler_f &handler_,
		const Completions::Completion_f &completion_fn = {},
		const std::vector<Completions::CompletionState::Suggestor_f> &suggestors_= {}):
			minArgs(min_args), maxArgs(max_args), needsServer(needs_server), handler(handler_),
			completionFunction(completion_fn), suggestors(suggestors_) {}

		Command(int min_args, int max_args, bool needs_server, const Handler_f &handler_,
		const Completions::Completion_f &completion_fn = {},
		const std::vector<Completions::CompletionState::Suggestor_f> &suggestors_= {}):
			minArgs(min_args), maxArgs(max_args), needsServer(needs_server), completionFunction(completion_fn),
			suggestors(suggestors_) {
				handler = [handler_](PingPong::Server *server, const InputLine &line) {
					handler_(server, line);
					return false;
				};
			}
	};

	/** Command name, command */
	using Pair = std::pair<std::string, Command>;

	void doAlias(Client &, const InputLine &);
	void doConnect(Client &, const InputLine &);
	void doDisconnect(Client &, PingPong::Server *, const InputLine &);
	void doJoin(Client &, PingPong::Server *, const InputLine &);
	void doMe(UI::Interface &, const InputLine &);
	void doMove(UI::Interface &, const InputLine &);
	void doMode(UI::Interface &, PingPong::Server *, const InputLine &);
	void doNick(UI::Interface &, PingPong::Server *, const InputLine &);
	void doPart(Client &, PingPong::Server *, const InputLine &);
	void doPlugin(Client &, const InputLine &);
	void doSet(Client &, const InputLine &);
	void doSpam(UI::Interface &, const InputLine &);
	void doTopic(Client &, PingPong::Server *, const InputLine &);
}

#endif
