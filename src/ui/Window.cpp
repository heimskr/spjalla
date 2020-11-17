#include "spjalla/ui/Window.h"
#include "pingpong/events/Event.h"
#include "spjalla/events/Notification.h"
#include "spjalla/lines/Basic.h"

namespace Spjalla::UI {
	void swap(Window &left, Window &right) {
		swap(static_cast<Haunted::UI::DequeBox &>(left), static_cast<Haunted::UI::DequeBox &>(right));
		std::swap(left.windowName, right.windowName);
		std::swap(left.type, right.type);
		std::swap(left.server, right.server);
		std::swap(left.channel, right.channel);
		std::swap(left.user, right.user);
		std::swap(left.highestNotification, right.highestNotification);
	}

	bool Window::shouldShowTimes() const {
		return type != WindowType::Overlay;
	}

	void Window::addLine(std::shared_ptr<Haunted::UI::DequeLine> line) {
		line->box = this;
		if (position.width != -1)
			doScroll(line->numRows(position.width));
		lines.push_back(line);
		rowsDirty();
		drawNewLine(*line, true);
	}

	Window &Window::operator+=(const std::string &str) {
		*this += Lines::BasicLine(str);
		return *this;
	}

	bool Window::isStatus() const {
		return type == WindowType::Status;
	}

	bool Window::isOverlay() const {
		return type == WindowType::Overlay;
	}

	bool Window::isChannel() const {
		return type == WindowType::Channel;
	}

	bool Window::isUser() const {
		return type == WindowType::User;
	}

	bool Window::isOther() const {
		return type == WindowType::Other;
	}

	bool Window::isDead() const {
		return dead;
	}

	void Window::kill() {
		dead = true;
	}

	void Window::resurrect() {
		dead = false;
	}

	void Window::notify(std::shared_ptr<Lines::Line> line, NotificationType type) {
		PingPong::Events::dispatch<Events::NotificationEvent>(this, line, type);
		if (highestNotification < type) {
			highestNotification = type;
			PingPong::Events::dispatch<Events::WindowNotificationEvent>(this, line, highestNotification, type);
		}
	}

	void Window::notify(std::shared_ptr<Lines::Line> line) {
		notify(line, line->getNotificationType());
	}

	void Window::unnotify() {
		if (highestNotification != NotificationType::None) {
			PingPong::Events::dispatch<Events::WindowNotificationEvent>(
				this, nullptr, highestNotification, NotificationType::None);
		}

		highestNotification = NotificationType::None;
	}

	void Window::removeRows(std::function<bool(const Haunted::UI::DequeLine *)> fn) {
		auto w = formicine::perf.watch("Window::remove_rows");
		std::deque<LinePtr> new_lines {};
		int rows_removed = 0, lines_removed = 0, total_rows = 0;

		for (LinePtr &ptr: lines) {
			int rows = ptr->numRows(position.width);
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
