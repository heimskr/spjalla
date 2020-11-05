#ifndef SPJALLA_PLUGINS_PLUGIN_H_
#define SPJALLA_PLUGINS_PLUGIN_H_

#include <functional>

#include "spjalla/commands/Command.h"
#include "spjalla/core/Util.h"

namespace Spjalla {
	class Client;
}

namespace Spjalla::Plugins {
	class PluginHost;

	/**
	 * Represents the priority of a plugin's handler for an event. Right now, there's no guarantee for how plugins with
	 * the same priority are ordered.
	 */
	enum class Priority: int {Low = 1, Normal = 2, High = 3};

	/**
	 * Indicates what should be done after handling an event.
	 * - `Pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `Kill` indicates that propagation to other plugins should be stopped.
	 */
	enum class HandlerResult {Pass, Kill};

	/**
	 * Indicates what should be done after handling a cancelable event.
	 * - `Pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `Kill` indicates that propagation to other plugins should be stopped. It also indicates that the cancelable
	 *   event shouldn't go through (like disable).
	 * - `Disable` indicates that the cancelable event shouldn't go through but continues propagation.
	 * - `Enable`  indicates that the cancelable event should go through and continues propagation.
	 * - `Approve` indicates that the cancelable event should go through (like enable) and stops propagation.
	 */
	enum class CancelableResult {Pass, Kill, Disable, Enable, Approve};

	/**
	 * Plugins modify the client's behavior. They reside in shared objects within a global variable called "ext_plugin".
	 */
	struct Plugin {
		Client *parent;

		virtual std::string getName()        const = 0;
		virtual std::string getDescription() const = 0;
		virtual std::string getVersion()     const = 0;

		/** Called when the plugin first loads, before client initialization. Useful for registering configurations. */
		virtual void preinit(PluginHost *) {}

		/** Called after client initialization. */
		virtual void postinit(PluginHost *) {}

		/** Called when the client is shutting down. */
		virtual void cleanup(PluginHost *) {}

		/** Tries to unload the plugin. Returns true if the plugin was successfully unloaded. */
		bool unload();
	};
}

#endif
