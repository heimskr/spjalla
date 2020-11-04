#ifndef SPJALLA_CORE_PluginHost_H_
#define SPJALLA_CORE_PluginHost_H_

#include <list>
#include <map>
#include <string>

#include "haunted/core/Key.h"

#include "pingpong/commands/Command.h"

#include "spjalla/core/InputLine.h"
#include "spjalla/plugins/Plugin.h"

namespace Spjalla::Plugins {
	class PluginHost {
		public:
			using PluginTuple = std::tuple<std::string, Plugins::Plugin *, void *>; // path, plugin

			template <typename T>
			// The bool argument indicates whether the result hasn't been disabled.
			using Pre_f = std::function<Plugins::CancelableResult(T &, bool)>;

			template <typename T>
			using Post_f = std::function<void(const T &)>;

			template <typename T>
			using PrePtr = std::weak_ptr<Pre_f<T>>;

			template <typename T>
			using PostPtr = std::weak_ptr<Post_f<T>>;

		private:
			std::list<PluginTuple> plugins {};
			std::map<Plugins::Priority, std::list<PrePtr<PingPong::Command>>> pluginCommandHandlers =
				{{Plugins::Priority::High, {}}, {Plugins::Priority::Normal, {}}, {Plugins::Priority::Low, {}}};

			/** Holds prehandlers for keypresses. Note that keypresses handled by the textinput aren't passed on to pre-
			 *  or posthandlers. */
			std::list<PrePtr<Haunted::Key>>  keyhandlersPre  {};

			/** Holds posthandlers for keypresses. Note that keypresses handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::list<PostPtr<Haunted::Key>> keyhandlersPost {};

			/** Holds prehandlers for input lines. Note that input lines handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::list<PrePtr<InputLine>>  inputhandlersPre  {};

			/** Holds posthandlers for input lines. Note that input lines handled by the textinput aren't passed on to
			 *  pre- or posthandlers. */
			std::list<PostPtr<InputLine>> inputhandlersPost {};

			/** Determines whether a pre-event should go through. */
			template <typename T>
			bool before(T &obj, const std::list<PrePtr<T>> &funcs) {
				return beforeMulti(obj, funcs).first;
			}

			/** Determines whether a pre-event should go through. Used inside functions like PluginHost::beforeSend
			 *  that process sets of sets of handler functions. */
			template <typename T>
			std::pair<bool, HandlerResult> beforeMulti(T &obj, const std::list<PrePtr<T>> &funcs,
			bool initial = true) {
				bool should_send = initial;
				for (auto &func: funcs) {
					if (func.expired()) {
						DBG("beforeMulti: pointer is expired");
						continue;
					}

					Plugins::CancelableResult result = (*func.lock())(obj, should_send);

					if (result == Plugins::CancelableResult::Kill || result == Plugins::CancelableResult::Disable) {
						should_send = false;
					} else if (result == Plugins::CancelableResult::Approve
					        || result == Plugins::CancelableResult::Enable) {
						should_send = true;
					}

					if (result == Plugins::CancelableResult::Kill || result == Plugins::CancelableResult::Approve)
						return {should_send, HandlerResult::Kill};
				}

				return {should_send, HandlerResult::Pass};
			}

			template <typename T>
			void after(const T &obj, const std::list<PostPtr<T>> &funcs) {
				for (auto &func: funcs) {
					if (func.expired()) {
						DBG("after: pointer is expired");
						continue;
					}

					(*func.lock())(obj);
				}
			}

			template <typename T>
			bool erase(std::list<T> &list, const T &item) {
				auto locked = item.lock();

				for (auto iter = list.begin(), end = list.end(); iter != end; ++iter) {
					if (iter->lock() == locked) {
						list.erase(iter);
						return true;
					}
				}

				return false;
			}

		public:
			virtual ~PluginHost() = 0;

			/** Unloads a plugin. */
			void unloadPlugin(PluginTuple &);

			/** Unloads all plugins. */
			void unloadPlugins();

			/** Loads a plugin from a given shared object. */
			PluginTuple loadPlugin(const std::string &path);

			/** Loads all plugins in a given directory. */
			void loadPlugins(const std::string &path);

			/** Returns a pointer to a plugin tuple by path or name. Returns nullptr if no match was found. */
			PluginTuple * getPlugin(const std::string &, bool insensitive = false);

			/** Returns a pointer to a plugin's tuple. Returns nullptr if no match was found. */
			PluginTuple * getPlugin(const Plugins::Plugin *);

			bool hasPlugin(const std::filesystem::path &) const;

			bool hasPlugin(const std::string &name, bool insensitive = false) const;

			/** Returns a const reference to the plugin list. */
			const std::list<PluginTuple> & getPlugins() const;

			/** Initializes all loaded plugins before client initialization. */
			void preinitPlugins();

			/** Initializes all loaded plugins after client initialization. */
			void postinitPlugins();

			/** Determines whether a command can be sent. Returns true if the command should be sent, or false if a
			 *  plugin chose to block it. */
			bool beforeSend(PingPong::Command &);

			/** Determines whether a key should be processed by the client. Returns true if so, or false if a plugin
			 *  chose to block the key. Can modify the input. */
			bool beforeKey(Haunted::Key &key) {
				return before(key, keyhandlersPre);
			}

			bool beforeInput(InputLine &il) {
				return before(il, inputhandlersPre);
			}

			void afterInput(const InputLine &il) {
				return after(il, inputhandlersPost);
			}

			/** If a plugin was loaded from a given path, a pointer to its corresponding plugin object is returned. */
			Plugins::Plugin * pluginForPath(const std::string &path) const;

			/** Registers a handler for commands. */
			void handle(const PrePtr<PingPong::Command> &func, Plugins::Priority priority) {
				pluginCommandHandlers[priority].push_back(func);
			}

			/** Registers a handler to handle keypresses before the client handles them and determine whether the client
			 *  will handle them. */
			void handle(const PrePtr<Haunted::Key> &func) {
				keyhandlersPre.push_back(func);
			}

			/** Registers a handler to handle keypresses after the client has handled them. */
			void handle(const PostPtr<Haunted::Key> &func) {
				// TODO: implement posthandlers for keypresses.
				keyhandlersPost.push_back(func);
			}

			/** Registers a handler to handle input lines before the client handles them and determine whether the
			 *  client will handle them. */
			void handle(const PrePtr<InputLine> &func) {
				inputhandlersPre.push_back(func);
			}

			/** Registers a handler to handle keypresses after the client has handled them. */
			void handle(const PostPtr<InputLine> &func) {
				inputhandlersPost.push_back(func);
			}

			void unhandle(const PrePtr<PingPong::Command> &func, Plugins::Priority priority) {
				erase(pluginCommandHandlers[priority], func);
			}

			void unhandle(const PrePtr<Haunted::Key> &func) {
				erase(keyhandlersPre, func);
			}

			void unhandle(const PostPtr<Haunted::Key> &func) {
				erase(keyhandlersPost, func);
			}

			void unhandle(const PrePtr<InputLine> &func) {
				erase(inputhandlersPre, func);
			}

			void unhandle(const PostPtr<InputLine> &func) {
				erase(inputhandlersPost, func);
			}

			// virtual void cleanup();
	};
}

#endif
