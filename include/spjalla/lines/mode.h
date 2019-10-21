#ifndef SPJALLA_LINES_MODE_H_
#define SPJALLA_LINES_MODE_H_

#include <unordered_map>

#include "pingpong/events/mode.h"

#include "spjalla/lines/line.h"

namespace spjalla::lines {
	class mode_line: public line {
		private:
			/** Maps a mode change (e.g., "+o", "-b") to a verb that indicates what the mode change means. */
			static std::unordered_map<std::string, std::string> verbs;

		public:
			pingpong::modeset mset;
			std::string where, who, self;

			mode_line(client *parent_, const pingpong::modeset &mset_, const std::string &where_,
			const std::string &who_, const std::string &self_, long stamp_):
				line(parent_, stamp_), mset(mset_), where(where_), who(who_) /*🦉*/, self(self_) {}

			mode_line(client *parent_, const pingpong::mode_event &ev):
				mode_line(parent_, ev.mset, ev.where, ev.who? ev.who->name : "", ev.serv->get_nick(), ev.stamp) {}

			virtual std::string render(ui::window *) override;
			virtual notification_type get_notification_type() const override { return notification_type::info; }
	};
}

#endif
