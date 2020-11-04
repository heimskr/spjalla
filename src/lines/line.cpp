#include <chrono>
#include <ctime>

#include "pingpong/core/Server.h"
#include "pingpong/core/Util.h"

#include "spjalla/core/Client.h"
#include "spjalla/lines/Line.h"
#include "spjalla/ui/Window.h"

#include "lib/formicine/ansi.h"

namespace Spjalla::Lines {
	std::string notice        = "\e[2m-\e[1m!\e[0;2m-\e[22m ";
	std::string redNotice    = "\e[31;2m-\e[22m!\e[2m-\e[22;39m ";
	std::string yellowNotice = "\e[33;2m-\e[22m!\e[2m-\e[22;39m ";

	std::string renderTime(Client *client, long stamp) {
		static long last_stamp = 0;
		static std::string last_result("?!?");

		if (stamp == last_stamp)
			return last_result;

		last_stamp = stamp;

		std::chrono::system_clock::time_point tpoint {PingPong::Util::TimeType(stamp)};
		std::time_t time = std::chrono::system_clock::to_time_t(tpoint);

		char str[33];
		if (0 == std::strftime(str, sizeof(str), client->cache.formatTimestamp.c_str(), std::localtime(&time)))
			return "???";

		return last_result = ansi::format(str) + " ";
	}

	long now() {
		return PingPong::Util::timestamp();
	}

	int Line::getContinuation() {
#ifndef RERENDER_LINES
		static int result = -1;

		if (result != -1)
			return result;
#endif

		if (UI::Window *win = dynamic_cast<UI::Window *>(box)) {
			if (!win->show_times())
				return baseContinuation;

			if (win->type == UI::WindowType::Status) {
				if (PingPong::Server *server = getAssociatedServer()) {
					return
#ifndef RERENDER_LINES
						result =
#endif
						ansi::length(renderTime(parent, stamp)) + 3 + server->id.length() + baseContinuation;
				}
			}
		}

		return
#ifndef RERENDER_LINES
			result =
#endif
			ansi::length(renderTime(parent, stamp)) + baseContinuation;
	}


	std::string Line::render() {
		UI::Window *win = dynamic_cast<UI::Window *>(box);

		if (box == nullptr) {
			DBG("box is null for " << render(nullptr));
			return renderTime(parent, stamp) + render(win);
		} else if (!win) {
			throw std::runtime_error("The box parent of a spjalla::lines::line must be a spjalla::UI::Window");
		}

		if (win->type == UI::WindowType::Status) {
			if (PingPong::Server *server = getAssociatedServer())
				return renderTime(parent, stamp) + "["_d + server->id + "] "_d + render(win);
		} else if (!win->show_times()) {
			return render(win);
		}

		return renderTime(parent, stamp) + render(win);
	}

	Line::operator std::string() {
#ifdef RERENDER_LINES
		return render();
#else
		return rendered.empty()? rendered = render() : rendered;
#endif
	}
}
