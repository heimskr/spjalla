#include "spjalla/config/cache.h"
#include "spjalla/ui/renderer.h"

namespace spjalla::ui {
	std::string renderer::nick(const std::string &nick, bool bright) const {
		std::string format = bright? cache->interface_nick_format_bright : cache->interface_nick_format;

		const size_t nick_pos = format.find("#n");
		if (nick_pos != std::string::npos) {
			format.erase(nick_pos, 2);
			format.insert(nick_pos, nick);
		}

		return ansi::format(format);
	}

	std::string renderer::nick(std::shared_ptr<pingpong::user> user,
	                                   std::shared_ptr<pingpong::channel> chan, bool bright) const {
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

	std::string renderer::channel(const std::string &channel) const {
		std::string format = cache->interface_channel_format;

		const size_t channel_pos = format.find("#c");
		if (channel_pos != std::string::npos) {
			format.erase(channel_pos, 2);
			format.insert(channel_pos, channel);
		}

		return ansi::format(format);
	}

	std::string renderer::channel(std::shared_ptr<pingpong::channel> chan) const {
		return channel(chan->name);
	}
}
