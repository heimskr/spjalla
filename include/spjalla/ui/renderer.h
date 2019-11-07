#ifndef SPJALLA_UI_RENDERER_H_
#define SPJALLA_UI_RENDERER_H_

#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"

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

		public:
			renderer(config::cache &cache_): cache(&cache_) {}

			/** Formats a nick according to interface.nick_format. */
			std::string nick(const std::string &, bool bright = false) const;

			/** Formats a nick according to interface.nick_format. */
			std::string nick(std::shared_ptr<pingpong::user>, std::shared_ptr<pingpong::channel>, bool bright = false)
				const;

			/** Formats a channel name according to interface.channel_format. */
			std::string channel(const std::string &) const;

			/** Formats a channel name according to interface.channel_format. */
			std::string channel(std::shared_ptr<pingpong::channel>) const;
	};
}

#endif
