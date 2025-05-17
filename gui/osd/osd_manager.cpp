#include "osd_manager.h"
#include <cstdio>

void OSDManager::add_widget(std::shared_ptr<OSDWidget> widget) {
    widgets.push_back(std::move(widget));
}

void OSDManager::update_all() {
    for (const auto& widget : widgets) {
        widget->update();
    }
}
