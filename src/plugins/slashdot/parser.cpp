#include <tinyxml2.h>

#include "spjalla/plugins/slashdot.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins::slashdot {
	std::vector<story> & parser::parse(const std::string &text) {
		tinyxml2::XMLDocument doc;
		doc.Parse(text.c_str(), text.length());
		tinyxml2::XMLElement *element = doc.FirstChildElement("backslash");
		for (element = element->FirstChildElement("story"); element != nullptr; element = element->NextSiblingElement()) {
			if (std::string(element->Name()) != "story")
				continue;

			try {
				stories.emplace_back(story {
					.title      = get_text(element->FirstChildElement("title")),
					.url        = get_text(element->FirstChildElement("url")),
					.author     = get_text(element->FirstChildElement("author")),
					.department = get_text(element->FirstChildElement("department")),
					.section    = get_text(element->FirstChildElement("section"))
				});

				story new_story = stories.back();
			} catch (const std::runtime_error &err) {
				DBG("Invalid story: " << err.what());
				continue;
			}
		}

		return stories;
	}

	std::string parser::get_text(tinyxml2::XMLElement *element) {
		if (!element)
			throw std::runtime_error("Element is null");
		return formicine::util::trim(element->GetText());
	}
}