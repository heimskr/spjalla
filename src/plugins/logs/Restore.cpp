#include "spjalla/core/Client.h"
#include "spjalla/lines/Line.h"
#include "spjalla/plugins/Logs.h"
#include "spjalla/ui/Interface.h"
#include "spjalla/ui/Window.h"
#include "spjalla/util/BackwardReader.h"

namespace Spjalla::Plugins::Logs {
	void LogsPlugin::restore(PingPong::Server *server, const InputLine &il) {
		UI::Interface &ui = parent->getUI();
		long to_restore;
		if (!il.args.empty()) {
			if (!formicine::util::parse_long(il.first(), to_restore)) {
				ui.error("Not a number: " + "\""_d + il.first() + "\"");
				return;
			}
		} else {
			to_restore = std::max(1L, parent->configs.get("logs", "default_restore").long_());
		}

		UI::Window *window = ui.getActiveWindow();
		if (window->type != UI::WindowType::Channel && window->type != UI::WindowType::User) {
			ui.warn("/restore works only for channel windows and private message windows.");
			return;
		}

		size_t first_stamp = static_cast<size_t>(-1);

		if (!window->getLines().empty()) {
			Lines::Line *line;
			for (const auto &lineptr: window->getLines()) {
				if ((line = dynamic_cast<Lines::Line *>(lineptr.get())))
					break;
			}

			if (!line)
				return;

			first_stamp = line->stamp;
		}

		PingPong::Util::TimeType first_time {first_stamp};

		const std::string where = window->isUser()? window->user->name : window->channel->name;
		const LogPair pair {server, where};
		if (filemap.count(pair) == 0) {
			ui.log("No scrollback found for " + ansi::bold(where) + " on " + ansi::bold(server->id) + ".");
			return;
		}

		std::vector<std::string> lines {};

		Util::BackwardReader reader(getPath(pair).string());

		// Look for the last line in the log before the top of the scrollback.
		for (;;) {
			size_t read = reader.readlines(lines, 1);
			if (read == 0) {
				ui.log("No more scrollback found for " + ansi::bold(where) + " on " + ansi::bold(server->id) + ".");
				break;
			}

			const std::string &line = lines.back();
			std::string first_word = line.substr(0, line.find(' '));
			
			std::chrono::microseconds micros = parseStamp(first_word);
			
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
		
		for (const std::string &raw: lines) {
			std::string first_word = formicine::util::nth_word(raw, 0, false);
			first_word.pop_back();
			long l;
			formicine::util::parse_long(first_word, l);

			auto line = getLine(pair, raw, autoclean);
			if (line) {
				line->box = window;
				line->clean(window->getPosition().width);
				window->getLines().push_front(std::move(line));
			}
		}

		window->rowsDirty();
		window->draw();
	}
}