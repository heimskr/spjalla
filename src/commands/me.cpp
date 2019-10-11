#include "pingpong/commands/privmsg.h"
#include "spjalla/commands/command.h"
#include "spjalla/ui/interface.h"

namespace spjalla::commands {
	void do_me(ui::interface &ui, const input_line &il) {
		const ui::window *win = ui.get_active_window();
		if (win->is_dead())
			return;

		const std::string msg = "\1ACTION " + il.body + "\1";
		if (win->is_channel())
			pingpong::privmsg_command(win->chan, msg).send();
		else if (win->is_user())
			pingpong::privmsg_command(win->user, msg).send();
	}
}
