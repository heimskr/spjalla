#ifndef SPJALLA_UI_RENDERER_H_
#define SPJALLA_UI_RENDERER_H_

#include "pingpong/core/channel.h"
#include "pingpong/core/user.h"
#include "strender/strnode.h"

namespace spjalla::config {
	class cache;
}

namespace spjalla::ui {
	/**
	 * Handles rendering of small bits of the interface, such as nicks in messages and channel names.
	 */
	class renderer {
		private:
			strender::strnode privmsg_header, action_header, notice_header;
			strender::strnode privmsg_message, action_message, notice_message;
			strender::strnode privmsg_nick, action_nick, notice_nick;

			config::cache *cache;

		public:
			/**
			 * normal:  In most places, like join messages.
			 * message: At the beginning of privmsgs, like the "someone" in "< someone>".
			 * action: At the beginning of actions, like the "someone" in "* someone".
			 * notice: In a notice.
			 */
			enum class nick_situation {normal, message, action, notice};

			strender::strnode message;
			strender::strnode action;
			strender::strnode notice;

			renderer(config::cache &);

			/** Performs initial setup of the strnodes. The setup process isn't complete until copy_strnodes() is
			 *  called. */
			void init_strnodes();

			/** Copies common strnodes to multiple parents. */
			void copy_strnodes();
	};
}

#endif
