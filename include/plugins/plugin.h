#ifndef SPJALLA_PLUGINS_PLUGIN_H_
#define SPJALLA_PLUGINS_PLUGIN_H_

#include <functional>

#include "commands/command.h"
#include "core/util.h"

namespace spjalla {
	class client;
}

namespace spjalla::plugins {
	/**
	 * Represents the priority of a plugin's handler for an event. Right now, there's no guarantee for how plugins with
	 * the same priority are ordered.
	 */
	enum class priority: int {high = 3, normal = 2, low = 1};

	/**
	 * Indicates what should be done after handling an event.
	 * - `pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `kill` indicates that propagation to other plugins should be stopped.
	 */
	enum class handler_result {pass, kill};

	/**
	 * Indicates what should be done after handling a cancelable event.
	 * - `pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `kill` indicates that propagation to other plugins should be stopped. It also indicates that the cancelable
	 *   event shouldn't go through (like disable).
	 * - `disable` indicates that the cancelable event shouldn't go through but continues propagation.
	 * - `enable`  indicates that the cancelable event should go through and continues propagation.
	 * - `approve` indicates that the cancelable event should go through (like enable) and stops propagation.
	 */
	enum class cancelable_result {pass, kill, disable, enable, approve};

	/**
	 * Plugins modify the client's behavior. They reside in shared objects within a global variable called "ext_plugin".
	 */
	struct plugin {
		client *parent;

		virtual std::string get_name()    const = 0;
		virtual std::string get_version() const = 0;

		/** Called when the plugin first loads. */
		virtual void startup() {}

		/** Called when the client is shutting down. */
		virtual void cleanup() {}
	};
}

#endif
