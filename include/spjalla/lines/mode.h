#ifndef SPJALLA_LINES_MODE_H_
#define SPJALLA_LINES_MODE_H_

#include <unordered_map>

#include "pingpong/events/Mode.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	class ModeLine: public Line {
		private:
			/** Maps a mode change (e.g., "+o", "-b") to a verb that indicates what the mode change means. */
			static std::unordered_map<std::string, std::string> verbs;

		public:
			PingPong::ModeSet modeSet;
			std::string where, who, self;

			ModeLine(Client *parent_, const PingPong::ModeSet &mset, const std::string &where_, const std::string &who_,
			const std::string &self_, long stamp_):
				Line(parent_, stamp_), modeSet(mset), where(where_), who(who_) /*🦉*/, self(self_) {}

			ModeLine(Client *parent_, const PingPong::ModeEvent &ev):
				ModeLine(parent_, ev.modeSet, ev.where, ev.who? ev.who->name : "", ev.server->getNick(), ev.stamp) {}

			virtual std::string render(UI::Window *) override;
			virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
