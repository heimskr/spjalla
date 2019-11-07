#include "spjalla/core/client.h"
#include "spjalla/lines/kick.h"

namespace spjalla::lines {
	std::string kick_line::render(ui::window *) {
		if (is_self) {
			return red_notice
				+ parent->get_ui().render.nick(whom, chan->name, ui::renderer::nick_situation::normal, false)
				+ " was kicked from " + parent->get_ui().render.channel(chan->name) + " by "
				+ parent->get_ui().render.nick(who, chan->name, ui::renderer::nick_situation::normal, true)
				+ " ["_d + reason + "]"_d;
		} else {
			return notice
				+ parent->get_ui().render.nick(whom, chan->name, ui::renderer::nick_situation::normal, false)
				+ " was kicked from " + parent->get_ui().render.channel(chan->name) + " by "
				+ parent->get_ui().render.nick(who, chan->name, ui::renderer::nick_situation::normal, true)
				+ " ["_d + reason + "]"_d;
		}
	}

	notification_type kick_line::get_notification_type() const {
		return is_self? notification_type::highlight : notification_type::info;
	}
}
