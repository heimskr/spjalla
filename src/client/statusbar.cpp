#include "spjalla/core/Client.h"

namespace Spjalla {
	void Client::sortWidgets() {
		statusWidgets.sort(
			[&](const std::shared_ptr<UI::StatusWidget> &first, const std::shared_ptr<UI::StatusWidget> &second) {
				return first->priority < second->priority;
			}
		);

		renderStatusbar();
	}

	void Client::addStatusWidget(std::shared_ptr<UI::StatusWidget> widget) {
		statusWidgets.push_back(widget);
		sortWidgets();
	}

	bool Client::removeStatusWidget(std::shared_ptr<UI::StatusWidget> widget) {
		auto iter = std::find(statusWidgets.begin(), statusWidgets.end(), widget);
		if (iter == statusWidgets.end())
			return false;
		statusWidgets.erase(iter);
		renderStatusbar();
		return true;
	}

	void Client::initStatusbar() {
		ui.updateStatusbarFunction = [&, this](UI::Window *win) {
			const bool overlay_visible = ui.overlayVisible();
			std::string rendered;

			for (std::shared_ptr<UI::StatusWidget> &widget: statusWidgets) {
				if (widget->visibleFor(win, overlay_visible)) {
					if (!rendered.empty())
						rendered.push_back(' ');
					
					rendered += widget->surround(widget->render(win, overlay_visible));
				}
			}

			ui.statusbar->setText(rendered);
		};
	}

	void Client::renderStatusbar() {
		ui.updateStatusbar();
	}
}
