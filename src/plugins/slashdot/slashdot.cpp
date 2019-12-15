#include <cpr/cpr.h>

#include "spjalla/core/client.h"
#include "spjalla/plugins/slashdot.h"

namespace spjalla::plugins {
	void slashdot_plugin::postinit(plugin_host *host) {
		parent = dynamic_cast<spjalla::client *>(host);
		if (!parent) { DBG("Error: expected client as plugin host"); return; }

		parent->add(".", 0, 0, false, [](pingpong::server *, const input_line &) {
			std::string raw = R"(<backslash xmlns:backslash="https://slashdot.org/backslash.dtd"> <script/> <story> <title> Sony Announces Plan To Publish PlayStation Games On Non-PS Consoles </title> <url> https://slashdot.org/story/19/12/10/2336221/sony-announces-plan-to-publish-playstation-games-on-non-ps-consoles </url> <time>2019-12-11 02:00:00</time> <author>BeauHD</author> <department>share-the-love</department> <topic>394</topic> <comments>0</comments> <section>games</section> <image>playstation_64.png</image> </story> <story> <title>Nikon Is Killing Its Authorized Repair Program</title> <url> https://slashdot.org/story/19/12/10/2237225/nikon-is-killing-its-authorized-repair-program </url> <time>2019-12-11 01:30:00</time> <author>BeauHD</author> <department>more-than-a-little-inconvenient</department> <topic>84</topic> <comments>7</comments> <section>technology</section> <image>business_64.png</image> </story> <story> <title> Vietnam's Richest Man Bets $2 Billion To Sell Cars To Americans </title> <url> https://slashdot.org/story/19/12/10/2228203/vietnams-richest-man-bets-2-billion-to-sell-cars-to-americans </url> <time>2019-12-11 00:50:00</time> <author>BeauHD</author> <department>ambitious-goals</department> <topic>1971</topic> <comments>9</comments> <section>technology</section> <image>transportation_64.png</image> </story> <story> <title> Chrome Now Warns You When Your Password Has Been Stolen </title> <url> https://slashdot.org/story/19/12/10/2221225/chrome-now-warns-you-when-your-password-has-been-stolen </url> <time>2019-12-11 00:10:00</time> <author>BeauHD</author> <department>better-password-protections</department> <topic>10103</topic> <comments>14</comments> <section>yro</section> <image>chrome_64.png</image> </story> <story> <title> Facebook Fired a Contractor Who Was Paid Thousands In Bribes To Reactivate Banned Ad Accounts </title> <url> https://slashdot.org/story/19/12/10/227229/facebook-fired-a-contractor-who-was-paid-thousands-in-bribes-to-reactivate-banned-ad-accounts </url> <time>2019-12-10 23:30:00</time> <author>BeauHD</author> <department>shady-business</department> <topic>48197</topic> <comments>8</comments> <section>technology</section> <image>facebook_64.png</image> </story> <story> <title> New Plundervolt Attack Impacts Intel Desktop, Server, and Mobile CPUs </title> <url> https://slashdot.org/story/19/12/10/2155231/new-plundervolt-attack-impacts-intel-desktop-server-and-mobile-cpus </url> <time>2019-12-10 22:50:00</time> <author>BeauHD</author> <department>secure-until-proven-otherwise</department> <topic>76</topic> <comments>15</comments> <section>it</section> <image>security_64.png</image> </story> <story> <title>Microsoft Teams is the First Office App For Linux</title> <url> https://slashdot.org/story/19/12/10/1715255/microsoft-teams-is-the-first-office-app-for-linux </url> <time>2019-12-10 22:10:00</time> <author>msmash</author> <department>marching-ahead</department> <topic>11</topic> <comments>28</comments> <section>linux</section> <image>microsoft_64100.png</image> </story> <story> <title> Architects Are Playing With the Future of Design in Video Games </title> <url> https://slashdot.org/story/19/12/10/1935235/architects-are-playing-with-the-future-of-design-in-video-games </url> <time>2019-12-10 21:31:00</time> <author>msmash</author> <department>moving-forward</department> <topic>64</topic> <comments>15</comments> <section>games</section> <image>games_64.png</image> </story> <story> <title>Iran's Internet Freedom is On Life Support</title> <url> https://slashdot.org/story/19/12/10/1558259/irans-internet-freedom-is-on-life-support </url> <time>2019-12-10 20:50:00</time> <author>msmash</author> <department>meanwhile-elsewhere-in-the-world</department> <topic>60</topic> <comments>42</comments> <section>yro</section> <image>censorship_64.png</image> </story> <story> <title> Google Releases Chrome 79 With New Features Including an Option To Freeze Tabs and Back-Forward Caching </title> <url> https://slashdot.org/story/19/12/10/1959205/google-releases-chrome-79-with-new-features-including-an-option-to-freeze-tabs-and-back-forward-caching </url> <time>2019-12-10 20:10:00</time> <author>msmash</author> <department>browser-updates</department> <topic>10103</topic> <comments>20</comments> <section>yro</section> <image>chrome_64.png</image> </story> <story> <title> Are You One Of Avast's 400 Million Users? This Is Why It Collects And Sells Your Web Habits. </title> <url> https://slashdot.org/story/19/12/10/1922243/are-you-one-of-avasts-400-million-users-this-is-why-it-collects-and-sells-your-web-habits </url> <time>2019-12-10 19:30:00</time> <author>msmash</author> <department>show-me-the-money</department> <topic>76</topic> <comments>16</comments> <section>it</section> <image>security_64.png</image> </story> <story> <title> Greeks Set To Face Heavy Fines If They Don't Spend 30% of Their Income Electronically </title> <url> https://slashdot.org/story/19/12/10/1614216/greeks-set-to-face-heavy-fines-if-they-dont-spend-30-of-their-income-electronically </url> <time>2019-12-10 18:50:00</time> <author>msmash</author> <department>stranger-things</department> <topic>253</topic> <comments>103</comments> <section>news</section> <image>money_64.png</image> </story> <story> <title>The Death of the Paper Ticket For Sporting Events</title> <url> https://slashdot.org/story/19/12/10/163220/the-death-of-the-paper-ticket-for-sporting-events </url> <time>2019-12-10 18:10:00</time> <author>msmash</author> <department>closer-look</department> <topic>84</topic> <comments>73</comments> <section>slashdot</section> <image>business_64.png</image> </story> <story> <title> Google Built Its Own Tiny HDMI 2.1 Box To Jump-Start 'the Next Generation of Android TV' </title> <url> https://slashdot.org/story/19/12/10/1713257/google-built-its-own-tiny-hdmi-21-box-to-jump-start-the-next-generation-of-android-tv </url> <time>2019-12-10 17:30:00</time> <author>msmash</author> <department>for-what-it-is-worth</department> <topic>14</topic> <comments>18</comments> <section>technology</section> <image>google_64.png</image> </story> <story> <title> Facebook Tells US Attorney General It's Not Prepared To Get Rid Of Encryption On WhatsApp And Messenger </title> <url> https://slashdot.org/story/19/12/10/1555243/facebook-tells-us-attorney-general-its-not-prepared-to-get-rid-of-encryption-on-whatsapp-and-messenger </url> <time>2019-12-10 16:50:00</time> <author>msmash</author> <department>tussle-continues</department> <topic>537</topic> <comments>100</comments> <section>it</section> <image>encryption_64.png</image> </story> </backslash>)";
			slashdot::parser slash;
			std::vector<slashdot::story> &stories = slash.parse(raw);
			if (stories.empty()) {
				DBG("No stories.");
				return;
			}

			for (const slashdot::story &story: stories) {
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

				res.text = formicine::util::trim(formicine::util::remove_html(res.text));
				DBG("[" << res.text << "]");
			}
		});
	}

	void slashdot_plugin::cleanup(plugin_host *) {
		parent->remove_command(".");
	}
}

spjalla::plugins::slashdot_plugin ext_plugin {};
