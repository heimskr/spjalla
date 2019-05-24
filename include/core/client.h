#ifndef CORE_SPJALLA_H_
#define CORE_SPJALLA_H_

#include "core/defs.h"

namespace spjalla {
	class client {
		private:
			std::shared_ptr<pingpong::irc> pp;

		public: 
			client(std::shared_ptr<pingpong::irc> irc_): pp(irc_) {
				add_listeners();
			}

			void input_worker(pingpong::server_ptr serv);
			void add_listeners();
	};
}

#endif
