#ifndef SPJALLA_LINES_LINES_H_
#define SPJALLA_LINES_LINES_H_

#include <string>

#include "pingpong/core/Util.h"
#include "haunted/ui/Textbox.h"

#include "spjalla/core/Notifications.h"
#include "spjalla/core/Options.h"

#include "lib/formicine/ansi.h"

namespace PingPong {
	class Server;
}

namespace Spjalla {
	class Client;
	namespace UI {
		class Window;
	}
}

namespace Spjalla::Lines {
	extern std::string notice, redNotice, yellowNotice;

	/** Renders a UNIX timestamp as an hours-minutes-seconds set. */
	std::string renderTime(Client *, long stamp);

	class Line: public Haunted::UI::DequeLine {

		protected:
			/** Returns whether the server the line is associated with, if any. */
			virtual PingPong::Server * getAssociatedServer() const { return nullptr; }
			virtual std::string render(UI::Window *) = 0;
			int baseContinuation = 0;

#ifndef RERENDER_LINES
			std::string rendered;
#endif
			std::string render();

		public:
			Client *parent;
			long stamp;

			Line(Client *parent_, long stamp_ = PingPong::Util::timestamp(), int base_continuation = 0):
				baseContinuation(base_continuation), parent(parent_), stamp(stamp_) {}

			int getContinuation() override;
			operator std::string() override;

			virtual NotificationType getNotificationType() const { return NotificationType::None; }
	};

	/** Returns the current timestamp in seconds. */
	long now();
}

#endif
