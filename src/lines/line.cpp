#include <chrono>
#include <ctime>

#include "pingpong/core/server.h"
#include "pingpong/core/util.h"

#include "spjalla/core/client.h"
#include "spjalla/lines/line.h"
#include "spjalla/ui/window.h"

#include "lib/formicine/ansi.h"

namespace spjalla::lines {
	std::string notice        = "\e[2m-\e[1m!\e[0;2m-\e[22m ";
	std::string red_notice    = "\e[31;2m-\e[22m!\e[2m-\e[22;39m ";
	std::string yellow_notice = "\e[33;2m-\e[22m!\e[2m-\e[22;39m ";

	std::string render_time(client *cli, long stamp) {
		static long last_stamp = 0;
		static std::string last_result("?!?");

		if (stamp == last_stamp)
			return last_result;

		last_stamp = stamp;

		std::chrono::system_clock::time_point tpoint {pingpong::util::timetype(stamp)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);

		char str[33];
		if (0 == std::strftime(str, sizeof(str), cli->cache.format_timestamp.c_str(), std::localtime(&time)))
			return "???";

		return last_result = ansi::format(str) + " ";
	}

	long now() {
		return pingpong::util::timestamp();
	}

	int line::get_continuation() {
#ifndef RERENDER_LINES
		static int result = -1;

		if (result != -1)
			return result;
#endif

		if (ui::window *win = dynamic_cast<ui::window *>(box)) {
			if (!win->show_times())
				return base_continuation;

			if (win->type == ui::window_type::status) {
				if (pingpong::server *serv = get_associated_server()) {
					return
#ifdef RERENDER_LINES
						result =
#endif
						render_time(parent, stamp).length() + 3 + serv->id.length() + base_continuation;
				}
			}
		}

		return
#ifdef RERENDER_LINES
			result =
#endif
			render_time(parent, stamp).length() + base_continuation;
	}


	std::string line::render() {
		ui::window *win = dynamic_cast<ui::window *>(box);

		if (box == nullptr) {
			DBG("box is null for " << render(nullptr));
			return render_time(parent, stamp) + render(win);
		} else if (!win) {
			throw std::runtime_error("The box parent of a spjalla::lines::line must be a spjalla::ui::window");
		}

		if (win->type == ui::window_type::status) {
			if (pingpong::server *serv = get_associated_server())
				return render_time(parent, stamp) + "["_d + serv->id + "] "_d + render(win);
		} else if (!win->show_times()) {
			return render(win);
		}

		return render_time(parent, stamp) + render(win);
	}

	line::operator std::string() {
#ifdef RERENDER_LINES
		return render();
#else
		return rendered.empty()? rendered = render() : rendered;
#endif
	}
}
