#ifndef SPJALLA_PLUGINS_SLASHDOT_H_
#define SPJALLA_PLUGINS_SLASHDOT_H_

#include "spjalla/plugins/Plugin.h"

namespace tinyxml2 {
	class XMLElement;
};

namespace Spjalla::Plugins {
#ifdef ENABLE_SLASHDOT
	namespace Slashdot {
		struct Story {
			std::string title, url, author, department, section, time;
			int comments;

			std::string text;
		};

		class Parser {
			private:
				std::string getText(tinyxml2::XMLElement *);
			public:
				std::vector<Story> stories {};
				void parse(const std::string &);
				void fetch();
		};
	}
#endif

	struct SlashdotPlugin: public Plugin {
		~SlashdotPlugin() {}

#ifdef ENABLE_SLASHDOT
		std::string getName()        const override { return "Slashdot"; }
		std::string getDescription() const override { return "Adds a Slashdot reader."; }
		std::string getVersion()     const override { return "0.0.1"; }
		void postinit(PluginHost *) override;
		void cleanup(PluginHost  *) override;
#else
		std::string getName()        const override { return "Slashdot (disabled)"; }
		std::string getDescription() const override { return "Dummy Slashdot plugin. "
		                                                     "Enable with ./configure --slashdot."; }
		std::string getVersion()     const override { return "0.0.0"; }
#endif
	};
}

#endif