#include "core/client.h"

namespace spjalla {
	void client::sort_widgets() {
		std::sort(status_widgets.begin(), status_widgets.end(),
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

	void client::init_statusbar() {
		ui.update_statusbar_fn = [&](ui::window *win) {
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
