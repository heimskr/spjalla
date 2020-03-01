#include "pingpong/events/event.h"
#include "spjalla/events/notification.h"
#include "spjalla/lines/basic.h"
#include "spjalla/ui/window.h"

namespace spjalla::ui {
	void swap(window &left, window &right) {
		swap(static_cast<Haunted::UI::textbox &>(left), static_cast<Haunted::UI::textbox &>(right));
		std::swap(left.window_name, right.window_name);
		std::swap(left.type, right.type);
		std::swap(left.serv, right.serv);
		std::swap(left.chan, right.chan);
		std::swap(left.user, right.user);
		std::swap(left.highest_notification, right.highest_notification);
	}

	bool window::show_times() const {
		return type != window_type::overlay;
	}

	void window::add_line(std::shared_ptr<Haunted::UI::textline> line) {
		do_scroll(line->num_rows(pos.width));
		lines.push_back(line);
		rows_dirty();
		draw_new_line(*line, true);
	}

	window & window::operator+=(const std::string &str) {
		*this += lines::basic_line(str);
		return *this;
	}

	bool window::is_status() const {
		return type == window_type::status;
	}

	bool window::is_overlay() const {
		return type == window_type::overlay;
	}

	bool window::is_channel() const {
		return type == window_type::channel;
	}

	bool window::is_user() const {
		return type == window_type::user;
	}

	bool window::is_other() const {
		return type == window_type::other;
	}

	bool window::is_dead() const {
		return dead;
	}

	void window::kill() {
		dead = true;
	}

	void window::resurrect() {
		dead = false;
	}

	void window::notify(std::shared_ptr<lines::line> line, notification_type type) {
		pingpong::events::dispatch<events::notification_event>(this, line, type);
		if (highest_notification < type) {
			highest_notification = type;
			pingpong::events::dispatch<events::window_notification_event>(this, line, highest_notification, type);
		}
	}

	void window::notify(std::shared_ptr<lines::line> line) {
		notify(line, line->get_notification_type());
	}

	void window::unnotify() {
		if (highest_notification != notification_type::none) {
			pingpong::events::dispatch<events::window_notification_event>(this, nullptr, highest_notification,
				notification_type::none);
		}

		highest_notification = notification_type::none;
	}

	void window::remove_rows(std::function<bool(const Haunted::UI::textline *)> fn) {
		auto w = formicine::perf.watch("window::remove_rows");
		std::deque<Haunted::UI::textbox::line_ptr> new_lines {};
		int rows_removed = 0, lines_removed = 0, total_rows = 0;

		for (Haunted::UI::textbox::line_ptr &ptr: lines) {
			int rows = ptr->num_rows(pos.width);
			total_rows += rows;
			if (fn(ptr.get())) {
				if (total_rows < voffset)
					rows_removed += rows;
				++lines_removed;
			} else {
				new_lines.push_back(std::move(ptr));
			}
		}

		lines.swap(new_lines);
		if (0 < rows_removed)
			vscroll(-rows_removed);
		if (0 < lines_removed)
			draw();
	}
}
