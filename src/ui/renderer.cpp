#include "spjalla/config/cache.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {


// Implementations


	std::string renderer::nick_impl(const std::string &nick, bool bright) const {
		std::string format = bright? cache->interface_nick_format_bright : cache->interface_nick_format;

		const size_t nick_pos = format.find("#n");
		if (nick_pos != std::string::npos) {
			format.erase(nick_pos, 2);
			format.insert(nick_pos, nick);
		}

		return ansi::format(format);
	}

	std::string renderer::nick_impl(std::shared_ptr<pingpong::user> user, std::shared_ptr<pingpong::channel> chan,
	                                bool bright) const {
		std::string format = bright? cache->interface_nick_format_bright : cache->interface_nick_format;

		const size_t hat_pos = format.find("#h");
		if (hat_pos != std::string::npos) {
			format.erase(hat_pos, 2);
			format.insert(hat_pos, std::string(chan->get_hats(user)));
		}

		const size_t nick_pos = format.find("#n");
		if (nick_pos != std::string::npos) {
			format.erase(nick_pos, 2);
			format.insert(nick_pos, user->name);
		}

		return ansi::format(format);
	}

	std::string renderer::channel_impl(const std::string &chan) const {
		std::string format = cache->interface_channel_format;

		const size_t channel_pos = format.find("#c");
		if (channel_pos != std::string::npos) {
			format.erase(channel_pos, 2);
			format.insert(channel_pos, chan);
		}

		return ansi::format(format);
	}

	std::string renderer::channel_impl(std::shared_ptr<pingpong::channel> chan) const {
		return channel(chan->name);
	}


// Wrappers


	std::string renderer::nick(const std::string &nick, bool bright) const {
		return provider_nick_str? provider_nick_str(nick, bright) : nick_impl(nick, bright);
	}

	std::string renderer::nick(std::shared_ptr<pingpong::user> user, std::shared_ptr<pingpong::channel> chan,
	                                bool bright) const {
		return provider_nick_ptr? provider_nick_ptr(user, chan, bright) : nick_impl(user, chan, bright);
	}

	std::string renderer::channel(const std::string &chan) const {
		return provider_channel_str? provider_channel_str(chan) : channel_impl(chan);
	}

	std::string renderer::channel(std::shared_ptr<pingpong::channel> chan) const {
		return provider_channel_ptr? provider_channel_ptr(chan) : channel_impl(chan);
	}


// Providers


	void renderer::provide_nick(renderer::fn_nick_str fn) { provider_nick_str = fn; }
	void renderer::provide_nick(renderer::fn_nick_ptr fn) { provider_nick_ptr = fn; }
	void renderer::provide_channel(renderer::fn_str         fn) { provider_channel_str = fn; }
	void renderer::provide_channel(renderer::fn_channel_ptr fn) { provider_channel_ptr = fn; }

}
