#ifndef SPJALLA_LINES_LINES_H_
#define SPJALLA_LINES_LINES_H_

#include <string>

#include "pingpong/core/util.h"
#include "haunted/ui/textbox.h"

#include "spjalla/core/notifications.h"
#include "spjalla/core/options.h"

#include "lib/formicine/ansi.h"

namespace pingpong {
	class server;
}

namespace spjalla {
	class client;
	namespace ui {
		class window;
	}
}

namespace spjalla::lines {
	extern std::string notice, red_notice, yellow_notice;

	/** Renders a UNIX timestamp as an hours-minutes-seconds set. */
	std::string render_time(client *, long stamp);

	class line: public Haunted::UI::textline {

		protected:
			/** Returns whether the server the line is associated with, if any. */
			virtual pingpong::server * get_associated_server() const { return nullptr; }
			virtual std::string render(ui::window *) = 0;
			int base_continuation = 0;

#ifndef RERENDER_LINES
			std::string rendered;
#endif
			std::string render();

		public:
			client *parent;
			long stamp;

			line(client *parent_, long stamp_ = pingpong::util::timestamp(), int base_continuation_ = 0):
				base_continuation(base_continuation_), parent(parent_), stamp(stamp_) {}

			int get_continuation() override;
			operator std::string() override;

			virtual notification_type get_notification_type() const { return notification_type::none; }
	};

	/** Returns the current timestamp in seconds. */
	long now();
}

#endif
