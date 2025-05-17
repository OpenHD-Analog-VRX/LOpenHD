#pragma once
#include <vector>
#include <memory>
#include "osd_widget.h"

class OSDManager {
public:
    void add_widget(std::shared_ptr<OSDWidget> widget);
    void update_all();

private:
    std::vector<std::shared_ptr<OSDWidget>> widgets;
};
