#ifndef SPJALLA_UI_RENDERER_H_
#define SPJALLA_UI_RENDERER_H_

#include <unordered_map>

#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"
#include "strender/strnode.h"

namespace spjalla::config {
	class cache;
}

namespace spjalla::ui {
	/**
	 * Handles rendering of small bits of the interface, such as nicks in messages and channel names.
	 */
	class renderer {
		private:
			config::cache *cache;

			/** Creates and inserts a group of strnodes for a simple message like joins, kicks, parts and quits. */
			void simple(const char *name, const std::string &format);

		public:
			std::unordered_map<std::string, strender::strnode> nodes {};

			renderer(config::cache &);

			/** Performs initial setup of the strnodes. The setup process isn't complete until copy_strnodes() is
			 *  called. */
			void init_strnodes();

			void more_strnodes();

			std::string channel(const std::string &);
	};
}

#endif
