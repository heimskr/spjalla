#ifndef SPJALLA_CORE_CLIENT_H_
#define SPJALLA_CORE_CLIENT_H_

#include <functional>
#include <list>
#include <map>
#include <thread>
#include <tuple>
#include <unordered_map>

#include "haunted/core/Defs.h"
#include "haunted/core/Key.h"

#include "pingpong/messages/Message.h"
#include "pingpong/core/IRC.h"
#include "pingpong/core/Server.h"


#include "spjalla/commands/Command.h"

#include "spjalla/core/Aliases.h"
#include "spjalla/core/InputLine.h"
#include "spjalla/core/PluginHost.h"
#include "spjalla/core/TabCompletion.h"

#include "spjalla/config/Cache.h"
#include "spjalla/config/Config.h"

#include "spjalla/plugins/Plugin.h"

#include "spjalla/ui/Interface.h"
#include "spjalla/ui/StatusWidget.h"

namespace Spjalla {
	class Client: public Plugins::PluginHost {
		friend class UI::Interface;

// client/client.cpp

		private:
			PingPong::IRC irc;
			std::multimap<std::string, Commands::Command> commandHandlers;
			std::mutex ircMutex;
			ansi::ansistream &outStream;
			Haunted::Terminal terminal;
			UI::Interface ui;
			Completions::CommandCompleter completer;

#include "Client.tcc"

		public:
			Config::Database configs;
			Aliases aliasDB;
			Config::Cache cache;

			Client(int heartbeat_period = PingPong::Util::precision / 10);

			Client(const Client &) = delete;
			Client(Client &&) = delete;
			Client & operator=(const Client &) = delete;
			Client & operator=(Client &&) = delete;
			~Client();

			/** Returns the ID of the active server. */
			std::string activeServerID();

			/** Returns the hostname of the active server. */
			std::string activeServerHostname();

			/**
			 * Adds a command handler.
			 * @param p A pair signifying the name of the command as typed by the user plus a handler tuple.
			 */
			Client & operator+=(const Spjalla::Commands::Pair &);

			/** Adds a server. */
			Client & operator+=(PingPong::Server *);

			/** Adds a command handler, given a pair that signifies the name of the command as typed by the user plus a
			 *  handler tuple. */
			void add(const Spjalla::Commands::Pair &);
			void add(const std::string &, const Spjalla::Commands::Command &);
			void add(const std::string &, int, int, bool, const Commands::Command::Handler_f &,
			         const Completions::Completion_f & = {},
			         const std::vector<Completions::CompletionState::Suggestor_f> & = {});

			bool removeCommand(const std::string &);

			/** Initializes the client. */
			void init();

			/** Performs actions after every plugin's postinit method has been called. */
			void postinit();

			/** Processes a line of user input and returns whether the line was recognized as a valid input. */
			bool handleLine(const InputLine &);

			/** Updates the interface to accommodate the removal of a server. */
			void serverRemoved(PingPong::Server *);

			/** Joins any threads associated with the client. */
			void join();

			/** Joins threads and unloads plugins. */
			void cleanup();

			/** Returns the client's UI::Interface instance. */
			UI::Interface & getUI() { return ui; }

			/** Returns a reference to the IRC object.. */
			PingPong::IRC & getIRC() { return irc; }

			/** Returns a reference to the terminal. */
			Haunted::Terminal & getTerminal() { return terminal; }

			/** Returns a pointer to the active server. */
			PingPong::Server * activeServer();

			/** Returns the nickname in use on the active server if possible, or a blank string otherwise. */
			std::string activeNick();

			/** Returns all the windows as Haunted::UI::Control pointers. */
			std::deque<Haunted::UI::Control *> getWindowControls() const;

			/** Opens a message window with a given nick. */
			UI::Window * query(const std::string &, PingPong::Server *);
			UI::Window * query(std::shared_ptr<PingPong::User>);

			/** Logs a message indicated that there is no active channel. */
			void noChannel();

// client/commands.cpp

		private:
			/** Handles the parsing for the /ban command. */
			void ban(PingPong::Server *, const InputLine &, const std::string &type = "+b");

		public:
			/** Adds the built-in command handlers. */
			void addCommands();

			/** Prints debug information about the server list to the log file. */
			void debugServers();

// client/events.cpp

		private:
			using Queue_f   = std::function<void()>;
			using QueuePair = std::pair<PingPong::Server::Stage, Queue_f>;
			std::unordered_map<PingPong::Server *, std::list<QueuePair>> serverStatusQueue {};

			/** Calls and removes all functions in the server status queue waiting for a given server and status. */
			void callInQueue(PingPong::Server *, PingPong::Server::Stage);

		public:
			/** Adds listeners for pingpong events. */
			void addEvents();

			/** Adds a function to a queue to be called when a server reaches a given stage. */
			void waitForServer(PingPong::Server *, PingPong::Server::Stage, const Queue_f &);

// client/heartbeat.cpp

		public:
			using HeartbeatListener = std::shared_ptr<std::function<void(int)>>;

		private:
			bool heartbeatAlive = false;

			/** A thread that executes actions at regular intervals. */
			std::thread heartbeat;

			/** The duration to wait between heartbeats. */
			int heartbeatPeriod;

			/** Contains all the functions to execute on each heartbeat. */
			std::list<HeartbeatListener> heartbeatListeners {};

			/** Keeps executing all the heartbeat listeners and waiting for the heartbeat period. Stops when
			 *  heartbeatAlive turns false. */
			void heartbeatLoop();

		public:
			/** Adds a function to the list of heartbeat listeners. The heartbeat period is passed as an argument. */
			void addHeartbeatListener(const HeartbeatListener &);

			/** Removes a function from the list of heartbeat listeners. */
			void removeHeartbeatListener(const HeartbeatListener &);

			/** Starts the heartbeat thread, which executes all the heartbeat listeners at regular intervals. */
			void initHeartbeat();

// client/input.cpp

			/** Adds a listener to the textinput that processes its contents. */
			void addInputListener();

			/** Tries to expand a command (e.g., "mod" → "mode"). Returns a vector of all matches. */
			std::vector<std::string> commandMatches(const std::string &);

			InputLine getInputLine(const std::string &) const;

// client/statusbar.cpp

		private:
			std::list<std::shared_ptr<UI::StatusWidget>> statusWidgets;

			/** Sorts all the statusbar widgets by priority. */
			void sortWidgets();

		public:
			void addStatusWidget(std::shared_ptr<UI::StatusWidget>);

			bool removeStatusWidget(std::shared_ptr<UI::StatusWidget>);

			const std::list<std::shared_ptr<UI::StatusWidget>> & getStatusWidgets() const { return statusWidgets; }

			void initStatusbar();

			void renderStatusbar();

// client/tab_completion.cpp

			std::unordered_map<std::string, Completions::CompletionState> completionStates;

			void tabComplete();

			/** Completes a message for a given cursor position. The word_offset parameter represents the index of the
			 *  word for which the completion suffix will be added. This can be set to a negative value to disable the
			 *  completion suffix. */
			void completeMessage(std::string &, size_t cursor, ssize_t word_offset = 0);

			void keyPostlistener(const Haunted::Key &);
	};
}

#endif
