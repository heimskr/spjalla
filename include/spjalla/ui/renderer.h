#ifndef SPJALLA_UI_RENDERER_H_
#define SPJALLA_UI_RENDERER_H_

#include <unordered_map>

#include "pingpong/core/Channel.h"
#include "pingpong/core/User.h"
#include "strender/StrNode.h"

namespace Spjalla::Config {
	class Cache;
}

namespace Spjalla::UI {
	/**
	 * Handles rendering of small bits of the interface, such as nicks in messages and channel names.
	 */
	class Renderer {
		private:
			Config::Cache *cache;

			/** Creates and inserts a group of strnodes for a simple message like joins, kicks, parts and quits. */
			void simple(const std::string &name, const std::string &format);

			std::string replaceNick(const std::string &varname) const;

		public:
			std::unordered_map<std::string, std::shared_ptr<Strender::StrNode>> nodes {};

			Renderer(Config::Cache &);

			/** Performs initial setup of the StrNodes. The setup process isn't complete until copy_StrNodes() is
			 *  called. */
			void initStrNodes();

			std::string bang();
			std::string bad();
			std::string good();
			std::string warn();

			void moreStrNodes();

			std::string operator()(const std::string &, const Strender::PieceMap &);
			std::string operator()(const std::string &);

			Strender::StrNode & operator[](const std::string &);

			std::string channel(const std::string &);
			std::string nick(const std::string &, bool bright = false);

			template <typename... Args>
			std::shared_ptr<Strender::StrNode> insert(const std::string &id, Args && ...args) {
				if (nodes.count(id) == 0)
					nodes.insert({id, std::make_shared<Strender::StrNode>(std::forward<Args>(args)...)});
				return nodes.at(id);
			}
	};
}

#endif
