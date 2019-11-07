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
			using fn_str         = std::function<std::string(const std::string &)>;
			using fn_nick_str    = std::function<std::string(const std::string &, bool)>;
			using fn_channel_ptr = std::function<std::string(std::shared_ptr<pingpong::channel>)>;
			using fn_nick_ptr =
				std::function<std::string(std::shared_ptr<pingpong::user>, std::shared_ptr<pingpong::channel>, bool)>;

		private:
			config::cache *cache;

			fn_nick_str    provider_nick_str;
			fn_nick_ptr    provider_nick_ptr;
			fn_str         provider_channel_str;
			fn_channel_ptr provider_channel_ptr;

		public:
			/**
			 * normal:  In most places, like join messages.
			 * message: At the beginning of messages.
			 */
			enum class nick_place {normal, message};

			renderer(config::cache &cache_): cache(&cache_) {}


// Wrappers

			/** Formats a nick according to interface.nick_format. */
			std::string nick(const std::string &, bool bright = false) const;

			/** Formats a nick according to interface.nick_format. */
			std::string nick(std::shared_ptr<pingpong::user>, std::shared_ptr<pingpong::channel>, bool bright = false)
				const;

			/** Formats a channel name according to interface.channel_format. */
			std::string channel(const std::string &) const;

			/** Formats a channel name according to interface.channel_format. */
			std::string channel(std::shared_ptr<pingpong::channel>) const;


// Implementations

			std::string nick_impl(const std::string &, bool bright = false) const;
			std::string nick_impl(std::shared_ptr<pingpong::user>, std::shared_ptr<pingpong::channel>,
				bool bright = false) const;
			std::string channel_impl(const std::string &) const;
			std::string channel_impl(std::shared_ptr<pingpong::channel>) const;


// Providers

			void provide_nick(fn_nick_str);
			void provide_nick(fn_nick_ptr);
			void provide_channel(fn_str);
			void provide_channel(fn_channel_ptr);
	};
}

#endif
