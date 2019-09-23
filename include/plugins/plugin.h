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
	 * Represents the priority of a plugin's handler for an event.
	 */
	enum class priority: int {high = 3, normal = 2, low = 1};

	/**
	 * Indicates what should be done after handling an event.
	 * - `pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `kill` indicates that propagation to other plugins should be stopped.
	 */
	enum class handle_result {pass, kill};

	/**
	 * Indicates what should be done after handling a command.
	 * - `pass` indicates that the plugin has chosen not to do anything with the event.
	 * - `kill` indicates that propagation to other plugins should be stopped. For commands, it indicates that the
	 *   command shouldn't go through (like disable).
	 * - `disable` indicates that the command shouldn't go through but continues propagation.
	 * - `enable` indicates that the command should go through and continues propagation.
	 * - `approve` indicates that the command should go through (like enable) and stops propagation.
	 */
	enum class command_result {pass, kill, disable, enable, approve};

	/**
	 * Plugins modify the client's behavior. They reside in shared objects within a global variable called "ext_plugin".
	 */
	struct plugin {
		client *parent;

		virtual ~plugin();

		virtual std::string get_name()    const = 0;
		virtual std::string get_version() const = 0;

		/** Called when the plugin first loads. */
		virtual void startup() {}

		/** Called when the client is shutting down. */
		virtual void cleanup() {}
	};
}

#endif
