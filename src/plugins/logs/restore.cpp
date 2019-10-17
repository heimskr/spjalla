#include "spjalla/core/client.h"
#include "spjalla/lines/line.h"
#include "spjalla/plugins/logs.h"
#include "spjalla/ui/interface.h"
#include "spjalla/ui/window.h"
#include "spjalla/util/backward_reader.h"

namespace spjalla::plugins::logs {
	void logs_plugin::restore(pingpong::server *serv, const input_line &il) {
		ui::interface &ui = parent->get_ui();
		long to_restore;
		if (!il.args.empty()) {
			if (!formicine::util::parse_long(il.first(), to_restore)) {
				ui.error("Not a number: " + "\""_d + il.first() + "\"");
				return;
			}
		} else {
			to_restore = std::max(1L, parent->configs.get("logs", "default_restore").long_());
		}

		ui::window *window = ui.get_active_window();
		if (window->type != ui::window_type::channel && window->type != ui::window_type::user) {
			ui.warn("/restore works only for channel windows and private message windows.");
			return;
		}

		size_t first_stamp = static_cast<size_t>(-1);

		if (!window->get_lines().empty()) {
			lines::line *line;
			for (const std::unique_ptr<haunted::ui::textline> &lineptr: window->get_lines()) {
				if ((line = dynamic_cast<lines::line *>(lineptr.get())))
					break;
			}

			if (!line)
				return;

			first_stamp = line->stamp;
		}

		pingpong::util::timetype first_time {first_stamp};

		const std::string where = window->is_user()? window->user->name : window->chan->name;
		const log_pair pair {serv, where};
		if (filemap.count(pair) == 0) {
			ui.log("No scrollback found for " + ansi::bold(where) + " on " + ansi::bold(serv->id) + ".");
			return;
		}

		std::vector<std::string> lines {};

		util::backward_reader reader(get_path(pair).string());

		// Look for the last line in the log before the top of the scrollback.
		for (;;) {
			size_t read = reader.readlines(lines, 1);
			if (read == 0) {
				ui.log("No more scrollback found for " + ansi::bold(where) + " on " + ansi::bold(serv->id) + ".");
				break;
			}

			const std::string &line = lines.back();
			std::string first_word = line.substr(0, line.find(' '));
			
			std::chrono::microseconds micros = parse_stamp(first_word);
			
			// Note: if there are messages in the unloaded scrollback that have an identical timestamp to the
			// first loaded line, they will be skipped. This can be mitigated with higher resolution.
			// With the current setting being microseconds, this is extremely unlikely to ever happen.

			if (micros < first_time) {
				break;
			} else {
				lines.pop_back();
			}
		}

		reader.readlines(lines, to_restore - 1);
		const bool autoclean = parent->configs.get("logs", "autoclean").bool_();

		ui::window *win = ui.get_active_window();
		
		for (const std::string &raw: lines) {
			std::string first_word = formicine::util::nth_word(raw, 0, false);
			first_word.pop_back();
			long l;
			formicine::util::parse_long(first_word, l);

			std::unique_ptr<haunted::ui::textline> line = get_line(pair, raw, autoclean);
			if (line) {
				line->box = win;
				win->get_lines().push_front(std::move(line));
			}
		}

		win->draw();
	}
}