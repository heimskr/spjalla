#include "spjalla/config/cache.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {


// Implementations


	std::string renderer::nick_impl(std::string format, const std::string &nick) const {
		const size_t nick_pos = format.find("#n");
		if (nick_pos != std::string::npos) {
			format.erase(nick_pos, 2);
			format.insert(nick_pos, nick);
		}

		return ansi::format(format);
	}

	std::string renderer::channel_impl(std::string format, const std::string &chan) const {
		const size_t channel_pos = format.find("#c");
		if (channel_pos != std::string::npos) {
			format.erase(channel_pos, 2);
			format.insert(channel_pos, chan);
		}

		return ansi::format(format);
	}


// Wrappers


	std::string renderer::nick(const std::string &nick, const std::string &where, nick_situation situation, bool bright)
	const {
		DBG((provider_nick? "nick fn provided" : "nick fn not provided"));
		std::string format = bright? cache->interface_nick_format_bright : cache->interface_nick_format;
		return provider_nick? provider_nick(format, nick, where, situation) : nick_impl(format, nick);
	}

	std::string renderer::channel(const std::string &chan) const {
		std::string format = cache->interface_channel_format;
		return provider_channel_str? provider_channel_str(format, chan) : channel_impl(format, chan);
	}

	std::string renderer::channel(std::shared_ptr<pingpong::channel> chan) const {
		std::string format = cache->interface_channel_format;
		return provider_channel_ptr? provider_channel_ptr(format, chan) : channel_impl(format, chan->name);
	}


// Providers


	void renderer::provide_nick   (renderer::fn_nick fn) { provider_nick = fn;        }
	void renderer::provide_channel(renderer::fn_str  fn) { provider_channel_str = fn; }
	void renderer::provide_channel(renderer::fn_cptr fn) { provider_channel_ptr = fn; }

}
