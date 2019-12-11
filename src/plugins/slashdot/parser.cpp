#include <tinyxml2.h>

#include "spjalla/plugins/slashdot.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins::slashdot {
	void parser::parse(const std::string &text) {
		tinyxml2::XMLDocument doc;
		doc.Parse(text.c_str(), text.length());
		tinyxml2::XMLElement *element = doc.FirstChildElement("backslash");
		for (element = element->FirstChildElement("story"); element != nullptr; element = element->NextSiblingElement()) {
			if (std::string(element->Name()) != "story")
				continue;
			
			story new_story;

			try {
				new_story.title      = get_text(element->FirstChildElement("title"));
				new_story.url        = get_text(element->FirstChildElement("url"));
				new_story.author     = get_text(element->FirstChildElement("author"));
				new_story.department = get_text(element->FirstChildElement("department"));
				new_story.section    = get_text(element->FirstChildElement("section"));
			} catch (const std::runtime_error &err) {
				DBG("Invalid story: " << err.what());
				continue;
			}

			DBG("title[" << new_story.title << "], url[" << new_story.url << "], author[" << new_story.author << "], department[" << new_story.department << "], section[" << new_story.section << "]");
		}
	}

	std::string parser::get_text(tinyxml2::XMLElement *element) {
		if (!element)
			throw std::runtime_error("Element is null");
		return formicine::util::trim(element->GetText());
	}
}