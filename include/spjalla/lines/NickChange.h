#ifndef SPJALLA_LINES_NICK_CHANGE_H_
#define SPJALLA_LINES_NICK_CHANGE_H_

#include "pingpong/core/Defs.h"
#include "pingpong/core/User.h"
#include "pingpong/core/Channel.h"

#include "pingpong/messages/Nick.h"

#include "pingpong/events/Nick.h"

#include "spjalla/lines/Line.h"

namespace Spjalla::Lines {
	struct NickChangeLine: public Line {
		std::shared_ptr<PingPong::User> user;
		const std::string oldName;
		const std::string newName;

		NickChangeLine(Client *parent_, std::shared_ptr<PingPong::User> user_, const std::string &old_name,
			const std::string &new_name, long stamp_):
			Line(parent_, stamp_), user(user_), oldName(old_name), newName(new_name) {}

		NickChangeLine(Client *parent_, const std::string &old_name, const std::string &new_name, long stamp_):
			NickChangeLine(parent_, nullptr, old_name, new_name, stamp_) {}

		NickChangeLine(Client *parent_, const PingPong::NickEvent &ev):
			NickChangeLine(parent_, ev.who, ev.content, ev.who->name, ev.stamp) {}

		virtual std::string render(UI::Window *) override;
		virtual NotificationType getNotificationType() const override { return NotificationType::Info; }
	};
}

#endif
