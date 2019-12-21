#ifndef SPJALLA_PLUGINS_SLASHDOT_H_
#define SPJALLA_PLUGINS_SLASHDOT_H_

#include "spjalla/plugins/plugin.h"

namespace tinyxml2 {
	class XMLElement;
};

namespace spjalla::plugins {
	namespace slashdot {
		struct story {
			std::string title, url, author, department, section, time;
			int comments;

			std::string text;
		};

		class parser {
			private:
				std::string get_text(tinyxml2::XMLElement *);
			public:
				std::vector<story> stories {};
				void parse(const std::string &);
				void fetch();
		};
	}

	struct slashdot_plugin: public plugin {
		~slashdot_plugin() {}

		std::string get_name()        const override { return "Slashdot"; }
		std::string get_description() const override { return "Adds a Slashdot reader."; }
		std::string get_version()     const override { return "0.0.1"; }

		void postinit(plugin_host *host) override;
		void cleanup(plugin_host *) override;
	};
}

#endif