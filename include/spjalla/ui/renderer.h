#ifndef SPJALLA_UI_RENDERER_H_
#define SPJALLA_UI_RENDERER_H_

#include <unordered_map>

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
			config::cache *cache;

			/** Creates and inserts a group of strnodes for a simple message like joins, kicks, parts and quits. */
			void simple(const std::string &name, const std::string &format);

			std::string replace_nick(const std::string &varname) const;

		public:
			std::unordered_map<std::string, std::shared_ptr<strender::strnode>> nodes {};

			renderer(config::cache &);

			/** Performs initial setup of the strnodes. The setup process isn't complete until copy_strnodes() is
			 *  called. */
			void init_strnodes();

			std::string bang();
			std::string bad();
			std::string good();
			std::string warn();

			void more_strnodes();

			std::string operator()(const std::string &, const strender::piece_map &);
			std::string operator()(const std::string &);

			strender::strnode & operator[](const std::string &);

			std::string channel(const std::string &);
			std::string nick(const std::string &, bool bright = false);

			template <typename... Args>
			std::shared_ptr<strender::strnode> insert(const std::string &id, Args && ...args) {
				if (nodes.count(id) == 0)
					nodes.insert({id, std::make_shared<strender::strnode>(std::forward<Args>(args)...)});
				return nodes.at(id);
			}
	};
}

#endif
