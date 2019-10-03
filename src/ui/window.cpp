#include "pingpong/events/event.h"
#include "spjalla/events/notification.h"
#include "spjalla/lines/line.h"
#include "spjalla/ui/window.h"

namespace spjalla::ui {
	void swap(window &left, window &right) {
		swap(static_cast<haunted::ui::textbox &>(left), static_cast<haunted::ui::textbox &>(right));
		std::swap(left.window_name, right.window_name);
		std::swap(left.data, right.data);
	}

	bool window::is_status() const {
		return data.type == window_type::status;
	}

	bool window::is_overlay() const {
		return data.type == window_type::overlay;
	}

	bool window::is_channel() const {
		return data.type == window_type::channel;
	}

	bool window::is_user() const {
		return data.type == window_type::user;
	}

	bool window::is_other() const {
		return data.type == window_type::other;
	}

	bool window::is_dead() const {
		return data.dead;
	}

	void window::kill() {
		data.dead = true;
	}

	void window::resurrect() {
		data.dead = false;
	}

	void window::notify(const lines::line &line, notification_type type) {
		pingpong::events::dispatch<events::notification_event>(this, &line, type);

		if (data.highest_notification < type) {
			data.highest_notification = type;
			pingpong::events::dispatch<events::window_notification_event>(this, &line, data.highest_notification, type);
		}
	}

	void window::notify(const lines::line &line) {
		notify(line, line.get_notification_type());
	}

	void window::unnotify() {
		if (data.highest_notification != notification_type::none) {
			pingpong::events::dispatch<events::window_notification_event>(this, nullptr, data.highest_notification,
				notification_type::none);
		}

		data.highest_notification = notification_type::none;
	}
}
