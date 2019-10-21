#include <chrono>
#include <ctime>

#include "pingpong/core/server.h"
#include "pingpong/core/util.h"

#include "spjalla/lines/line.h"
#include "spjalla/ui/window.h"

#include "lib/formicine/ansi.h"

namespace spjalla::lines {
	std::string notice        = "\e[2m-\e[1m!\e[0;2m-\e[22m ";
	std::string red_notice    = "\e[31;2m-\e[22m!\e[2m-\e[22;39m ";
	std::string yellow_notice = "\e[33;2m-\e[22m!\e[2m-\e[22;39m ";
	size_t time_length = render_time(0, false).length() + 1; // The extra space is added only when with_ansi is true.

	std::string render_time(long stamp, bool with_ansi) {
		if (stamp == 0)
			return with_ansi? "["_d + "__:__:__" + "]"_d : "[__:__:__]";

		std::chrono::system_clock::time_point tpoint {pingpong::util::timetype(stamp)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);
		char str[64];
		std::strftime(str, sizeof(str), "%H:%M:%S", std::localtime(&time));
		return with_ansi? "["_d + str + "] "_d : "[" + std::string(str) + "]";
	}

	long now() {
		return pingpong::util::timestamp();
	}

	std::string line::to_string(haunted::ui::textbox *tbox) {
		ui::window *win = dynamic_cast<ui::window *>(tbox);

		if (tbox == nullptr) {
			DBG("tbox is null for " << render(nullptr));
			return render_time(stamp, true) + render(win);
		} else if (!win) {
			throw std::runtime_error("A subclass of spjalla::lines::line must be rendered for a spjalla::ui::window");
		}

		if (win->type == ui::window_type::status) {
			if (pingpong::server *serv = get_associated_server())
				return render_time(stamp, true) + "["_d + serv->id + "] "_d + render(win);
		} else if (!win->show_times()) {
			return render(win);
		}

		return render_time(stamp, true) + render(win);
	}
}
