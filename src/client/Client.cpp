#include <iostream>
#include <string>

#include "spjalla/core/Options.h"

#include "haunted/core/Util.h"

#include "pingpong/core/Channel.h"
#include "pingpong/core/Debug.h"
#include "pingpong/core/Defs.h"
#include "pingpong/core/IRC.h"
#include "pingpong/core/Server.h"

#include "pingpong/commands/Mode.h"

#include "pingpong/net/ResolutionError.h"

#include "spjalla/core/Client.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/config/Config.h"
#include "spjalla/config/Defaults.h"

#include "spjalla/lines/Line.h"

#include "lib/formicine/ansi.h"
#include "lib/formicine/performance.h"

namespace Spjalla {
	Client::Client(int heartbeat_period): outStream(ansi::out), terminal(Haunted::Terminal(std::cin, outStream)),
	ui(terminal, *this), completer(*this), configs({*this, false}), heartbeatPeriod(heartbeat_period) {
		Config::registerDefaults();
		DBG("Set up configs.");
		terminal.keyPostlistener = [this](const Haunted::Key &k) { keyPostlistener(k); };
	}

	Client::~Client() {
		statusWidgets.clear();
		heartbeatListeners.clear();
		commandHandlers.clear();
		unloadPlugins();
	}


// Public instance methods


	std::string Client::activeServerID() {
		return irc.activeServer? irc.activeServer->id : "";
	}

	std::string Client::activeServerHostname() {
		return irc.activeServer? irc.activeServer->hostname : "";
	}


	Client & Client::operator+=(const Spjalla::Commands::Pair &p) {
		add(p);
		return *this;
	}
	
	Client & Client::operator+=(PingPong::Server *ptr) {
		irc += ptr;
		return *this;
	}

	void Client::add(const Spjalla::Commands::Pair &p) {
		commandHandlers.insert(p);
		completionStates.insert({p.first, Completions::CompletionState(p.second.suggestors)});
	}

	void Client::add(const std::string &command_name, const Commands::Command &command) {
		add({command_name, command});
	}

	void Client::add(const std::string &command_name, int min_args, int max_args, bool needs_server,
	                 const Commands::Command::Handler_f &handler_fn,
	                 const Completions::Completion_f &completion_fn,
	                 const std::vector<Completions::CompletionState::Suggestor_f> &suggestors) {
		add({command_name, {min_args, max_args, needs_server, handler_fn, completion_fn, suggestors}});
	}

	void Client::addBool(const std::string &command_name, int min_args, int max_args, bool needs_server,
	                     const Commands::Command::BoolHandler_f &handler_fn,
	                     const Completions::Completion_f &completion_fn,
	                     const std::vector<Completions::CompletionState::Suggestor_f> &suggestors) {
		add({command_name, {min_args, max_args, needs_server, handler_fn, completion_fn, suggestors}});
	}

	bool Client::removeCommand(const std::string &name) {
		if (commandHandlers.count(name) == 0)
			return false;
		commandHandlers.erase(name);
		return true;
	}

	void Client::init() {
		configs.readIfEmpty(DEFAULT_CONFIG_DB, true);
		aliasDB.readIfEmpty(DEFAULT_ALIAS_DB, true);
		ui.init();
		irc.init();
		addEvents();
		addCommands();
		addInputListener();
		terminal.startInput();
		initHeartbeat();
		initStatusbar();
	}

	void Client::postinit() {
		ui.postinit();
	}

	void Client::serverRemoved(PingPong::Server *server) {
		callInQueue(server, PingPong::Server::Stage::Dead);
		serverStatusQueue.erase(server);

		// We need to check the windows in reverse because we're removing some along the way. Removing elements while
		// looping through a vector causes all kinds of problems unless you loop in reverse.
		Haunted::UI::Container::Type &windows = ui.swappo->getChildren();
		for (auto iter = windows.rbegin(), rend = windows.rend(); iter != rend; ++iter) {
			UI::Window *window = dynamic_cast<UI::Window *>(*iter);
			if (window->server == server)
				ui.removeWindow(window);
		}
	}

	void Client::join() {
		terminal.join();
		if (heartbeat.joinable()) {
			heartbeatAlive = false;
			heartbeat.join();
		}
	}

	void Client::cleanup() {
		// PluginHost::cleanup();
		commandHandlers.clear();
		heartbeatListeners.clear();
		statusWidgets.clear();
		unloadPlugins();
		terminal.alive = false;
		formicine::perf.results();
	}

	PingPong::Server * Client::activeServer() {
		UI::Window *window = ui.activeWindow;
		if (window && window->server)
			return window->server;
		return irc.activeServer;
	}

	std::string Client::activeNick() {
		if (PingPong::Server *server = activeServer())
			return server->getNick();
		return "";
	}

	std::deque<Haunted::UI::Control *> Client::getWindowControls() const {
		return ui.swappo->getChildren();
	}

	UI::Window * Client::query(const std::string &name, PingPong::Server *server) {
		if (!server)
			server = activeServer();

		if (!server)
			throw std::runtime_error("Can't query user: server is null");
		return query(server->getUser(name, true, false));
	}

	UI::Window * Client::query(std::shared_ptr<PingPong::User> whom) {
		return ui.getWindow(whom, true);
	}

	void Client::noChannel() {
		ui.log(Lines::redNotice + "No active channel.");
	}
}
