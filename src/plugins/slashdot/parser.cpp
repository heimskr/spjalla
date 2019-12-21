#include <cpr/cpr.h>
#include <tinyxml2.h>

#include "spjalla/plugins/slashdot.h"
#include "lib/formicine/futil.h"

namespace spjalla::plugins::slashdot {
	void parser::parse(const std::string &text) {
		tinyxml2::XMLDocument doc;

		size_t wbr_index = text.find("<wbr>");
		// size_t nobr_index = text.find("<nobr> </nobr>");
		if (wbr_index == std::string::npos) {
			doc.Parse(text.c_str(), text.length());
		} else {
			std::string copy;
			copy.reserve(text.size());
			copy.append(text.substr(0, wbr_index));
			while (wbr_index != std::string::npos) {
				const size_t next_index = text.find("<wbr>", wbr_index + 1);
				copy.append(text.substr(wbr_index + 5, next_index != std::string::npos? next_index - (wbr_index + 5) : next_index));
				wbr_index = next_index;
			}
			DBG(copy);
			doc.Parse(copy.c_str(), copy.length());
		}

		tinyxml2::XMLElement *element = doc.RootElement();

		if (doc.Error()) {
			DBG(doc.ErrorStr());
			return;
		}

		if (!element) {
			DBG("Couldn't find root element in \"" << text.substr(0, 32) << "...\"");
			return;
		}

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
	}

	void parser::fetch() {
		for (story &story: stories) {
			DBG("Retrieving " << story.url << "...");
			auto res = cpr::Get(cpr::Url(story.url));
			DBG("Done.");
			if (res.status_code != 200) {
				DBG("Status: " << res.status_code);
				continue;
			}

			const size_t text_div = res.text.find("<div id=\"text-");
			if (text_div == std::string::npos) {
				DBG("Couldn't find text div.");
				continue;
			}

			res.text.erase(0, text_div);

			const size_t div_end = res.text.find("</div>");
			if (div_end == std::string::npos) {
				DBG("Couldn't find </div>.");
				continue;
			}

			res.text.erase(div_end);

			const size_t newline = res.text.find("\n");
			if (newline == std::string::npos) {
				DBG("Couldn't find newline.");
				continue;
			}

			res.text.erase(0, newline);
			story.text = formicine::util::trim(formicine::util::remove_html(res.text));
		}
	}

	std::string parser::get_text(tinyxml2::XMLElement *element) {
		if (!element)
			throw std::runtime_error("Element is null");
		return formicine::util::trim(element->GetText());
	}
}