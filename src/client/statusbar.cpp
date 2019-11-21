#include "spjalla/core/client.h"

namespace spjalla {
	void client::sort_widgets() {
		status_widgets.sort(
			[&](const std::shared_ptr<ui::status_widget> &first, const std::shared_ptr<ui::status_widget> &second) {
				return first->priority < second->priority;
			}
		);

		render_statusbar();
	}

	void client::add_status_widget(std::shared_ptr<ui::status_widget> widget) {
		status_widgets.push_back(widget);
		sort_widgets();
	}

	bool client::remove_status_widget(std::shared_ptr<ui::status_widget> widget) {
		auto iter = std::find(status_widgets.begin(), status_widgets.end(), widget);
		if (iter == status_widgets.end())
			return false;
		status_widgets.erase(iter);
		render_statusbar();
		return true;
	}

	void client::init_statusbar() {
		ui.update_statusbar_fn = [&, this](ui::window *win) {
			const bool overlay_visible = ui.overlay_visible();
			std::string rendered;

			for (std::shared_ptr<ui::status_widget> &widget: status_widgets) {
				if (widget->visible_for(win, overlay_visible)) {
					if (!rendered.empty())
						rendered.push_back(' ');
					
					rendered += widget->surround(widget->render(win, overlay_visible));
				}
			}

			ui.statusbar->set_text(rendered);
		};
	}

	void client::render_statusbar() {
		ui.update_statusbar();
	}
}
