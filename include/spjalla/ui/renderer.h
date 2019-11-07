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
		public:
			/**
			 * normal:  In most places, like join messages.
			 * message: At the beginning of privmsgs, like the "someone" in "< someone>".
			 * action: At the beginning of actions, like the "someone" in "* someone".
			 * notice: In a notice.
			 */
			enum class nick_situation {normal, message, action, notice};

			// Original format, some string.
			using fn_str  = std::function<std::string(std::string, const std::string &)>;

			// Original format, channel.
			using fn_cptr = std::function<std::string(std::string, std::shared_ptr<pingpong::channel>)>;

			// Original format, nick, location of message, situation.
			using fn_nick =
				std::function<std::string(std::string, const std::string &, const std::string &, nick_situation)>;

			renderer(config::cache &cache_): cache(&cache_) {}


// Wrappers

			std::string nick(const std::string &nick, const std::string &where, nick_situation, bool bright = false)
				const;
			std::string channel(const std::string &) const;
			std::string channel(std::shared_ptr<pingpong::channel>) const;


// Implementations

			std::string nick_impl(std::string format, const std::string &nick) const;
			std::string channel_impl(std::string format, const std::string &chan) const;


// Providers

			void provide_nick(fn_nick);
			void provide_channel(fn_str);
			void provide_channel(fn_cptr);


		private:
			config::cache *cache;

			fn_nick provider_nick;
			fn_str  provider_channel_str;
			fn_cptr provider_channel_ptr;
	};
}

#endif
