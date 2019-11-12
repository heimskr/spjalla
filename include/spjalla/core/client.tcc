private:
	template <typename T>
	ui::window * try_window(const T &where) {
		ui::window *win = ui.get_window(where, false);
		if (!win)
			ui.log(ansi::yellow("!!") + " Couldn't find window for " + std::string(*where));
		return win;
	}

	/** Handles commands like /kick that take a user and an optional longer string and an optional channel.
	 *  If no channel is specified, the command must be issued from a channel window.
	 *  Returns whether a "no active channel" message should be displayed. */
	template <typename T>
	bool triple_command(pingpong::server *serv, const input_line &il, std::shared_ptr<pingpong::channel> chan) {
		const std::string first = il.first(), rest = il.rest();
		const size_t spaces = std::count(il.body.begin(), il.body.end(), ' ');

		if (first.front() == '#') {
			const std::string &where = first;
			if (spaces == 1) {
				T(serv, where, rest).send();
			} else {
				const size_t found = rest.find(' ');
				T(serv, where, rest.substr(0, found), rest.substr(found + 1)).send();
			}
		} else if (chan) {
			T(serv, chan, first, rest).send();
		} else {
			return true;
		}

		return false;
	}

public:
	/**
	 * Succinctly adds a handler for a single-argument command.
	 * @param cmd The name of the command as typed by the user.
	 */
	template <typename T>
	void add(const std::string &cmd, bool needs_serv = true) {
		*this += {cmd, {1, 1, needs_serv, [&](pingpong::server *serv, const input_line &il) {
			T(serv, il.args[0]).send();
		}, {}}};
	}
	
	template <typename T>
	void log(const T &obj) {
		ui.log(obj);
	}

	template <typename T, typename P>
	void log(const T &obj, P *ptr) {
		ui.log(obj, ptr);
	}
