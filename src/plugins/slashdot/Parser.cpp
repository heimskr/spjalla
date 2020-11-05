#include <cpr/cpr.h>
#include <thread>
#include <tinyxml2.h>

#include "spjalla/plugins/Slashdot.h"
#include "lib/formicine/futil.h"

namespace Spjalla::Plugins::Slashdot {
	void Parser::parse(const std::string &text) {
		tinyxml2::XMLDocument doc;

		size_t wbr_index = text.find("<wbr>");
		size_t nobr_index = text.find("<nobr> </nobr>");
		if (wbr_index == std::string::npos) {
			doc.Parse(text.c_str(), text.length());
		} else {
			// Aggressively fix up some of the garbage I've seen end up in Slashdot titles, e.g.
			// https://tech.slashdot.org/story/19/12/16/2133236/

			std::string copy = text;

			while (wbr_index != std::string::npos) {
				copy.erase(wbr_index, 5);
				wbr_index = copy.find("<wbr>");
			}

			while (nobr_index != std::string::npos) {
				copy.erase(nobr_index, 14);
				nobr_index = copy.find("<nobr> </nobr>");
			}

			nobr_index = copy.find("<nobr>");
			while (nobr_index != std::string::npos) {
				copy.erase(nobr_index, 6);
				nobr_index = copy.find("<nobr>");
			}

			nobr_index = copy.find("</nobr>");
			while (nobr_index != std::string::npos) {
				copy.erase(nobr_index, 7);
				nobr_index = copy.find("</nobr>");
			}

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
				stories.emplace_back(Story {
					.title      = getText(element->FirstChildElement("title")),
					.url        = getText(element->FirstChildElement("url")),
					.author     = getText(element->FirstChildElement("author")),
					.department = getText(element->FirstChildElement("department")),
					.section    = getText(element->FirstChildElement("section")),
					.time       = getText(element->FirstChildElement("time"))
				});

				Story new_story = stories.back();
			} catch (const std::runtime_error &err) {
				DBG("Invalid story: " << err.what());
				continue;
			}
		}
	}

	void Parser::fetch() {
		std::vector<std::thread> threads;

		for (Story &story: stories) {
			threads.emplace_back([&story]() {
				DBG("Retrieving " << story.url << "...");
				auto res = cpr::Get(cpr::Url(story.url));
				DBG("Done.");
				if (res.status_code != 200) {
					DBG("Status: " << res.status_code);
					return;
				}

				const size_t text_div = res.text.find("<div id=\"text-");
				if (text_div == std::string::npos) {
					DBG("Couldn't find text div.");
					return;
				}

				res.text.erase(0, text_div);

				const size_t div_end = res.text.find("</div>");
				if (div_end == std::string::npos) {
					DBG("Couldn't find </div>.");
					return;
				}

				res.text.erase(div_end);

				const size_t newline = res.text.find("\n");
				if (newline == std::string::npos) {
					DBG("Couldn't find newline.");
					return;
				}

				res.text.erase(0, newline);
				story.text = formicine::util::trim(formicine::util::remove_html(res.text));
			});
		}

		for (std::thread &thread: threads)
			thread.join();
	}

	std::string Parser::getText(tinyxml2::XMLElement *element) {
		if (!element)
			throw std::runtime_error("Element is null");
		return formicine::util::trim(element->GetText());
	}
}