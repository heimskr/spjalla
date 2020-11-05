private:
	template <typename T>
	UI::Window * tryWindow(const T &where) {
		UI::Window *win = ui.getWindow(where, false);
		if (!win)
			ui.log(ansi::yellow("!!") + " Couldn't find window for " + std::string(*where));
		return win;
	}

	/** Handles commands like /kick that take a user and an optional longer string and an optional channel.
	 *  If no channel is specified, the command must be issued from a channel window.
	 *  Returns whether a "no active channel" message should be displayed. */
	template <typename T>
	bool tripleCommand(PingPong::Server *serv, const InputLine &il, std::shared_ptr<PingPong::Channel> chan) {
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
		*this += {cmd, {1, 1, needs_serv, [&](PingPong::Server *serv, const InputLine &il) {
			T(serv, il.args[0]).send();
		}, {}}};
	}
	
	template <typename T>
	void log(const T &obj) {
		ui.log(obj);
	}

	void log(const char *str) {
		ui.log(std::string(str));
	}

	template <typename T, typename P>
	void log(const T &obj, P *ptr) {
		ui.log(obj, ptr);
	}

	template <typename T>
	void error(const T &obj) {
		ui.error(obj);
	}

	void error(const char *str) {
		ui.error(std::string(str));
	}

	template <typename T, typename P>
	void error(const T &obj, P *ptr) {
		ui.error(obj, ptr);
	}

	template <typename T>
	void warn(const T &obj) {
		ui.warn(obj);
	}

	void warn(const char *str) {
		ui.warn(std::string(str));
	}

	template <typename T, typename P>
	void warn(const T &obj, P *ptr) {
		ui.warn(obj, ptr);
	}

	template <typename T>
	void success(const T &obj) {
		ui.success(obj);
	}

	void success(const char *str) {
		ui.success(std::string(str));
	}

	template <typename T, typename P>
	void success(const T &obj, P *ptr) {
		ui.success(obj, ptr);
	}
