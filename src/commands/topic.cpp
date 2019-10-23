#include "pingpong/core/channel.h"

#include "spjalla/commands/command.h"
#include "spjalla/core/client.h"
#include "spjalla/lines/topic.h"

namespace spjalla::commands {
	void do_topic(client &cli, pingpong::server *serv, const input_line &il) {
		ui::interface &ui = cli.get_ui();
		if (il.args.size() < 2) {
			std::shared_ptr<pingpong::channel> chan = il.args.empty()?
				ui.get_active_channel() : serv->get_channel(il.first(), false);
			if (!chan) {
				ui.warn("No channel selected.");
				return;
			}

			ui::window *win = ui.get_window(chan, false);
			if (!win)
				win = ui.get_window();

			ui.log(lines::topic_line(&cli, "", chan->name, chan->topic, pingpong::util::timestamp()), win);
		}
	}
}
