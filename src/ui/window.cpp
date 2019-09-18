#include "ui/window.h"

namespace spjalla::ui {
	void swap(window &left, window &right) {
		swap(static_cast<haunted::ui::textbox &>(left), static_cast<haunted::ui::textbox &>(right));
		std::swap(left.window_name, right.window_name);
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
}
