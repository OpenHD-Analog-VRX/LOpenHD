#pragma once
#include <string>

#include "lvgl.h"

class OSDWidget {
public:
    virtual void update() = 0;

    virtual void set_visible(bool visible);
    virtual void set_position(int x, int y);
    virtual void set_color(lv_color_t color);

    // Shared icon/text support
    virtual void set_text_prefix(const std::string& prefix);
    virtual void set_icon_src(const std::string& icon_path);
    virtual void set_show_unit(bool show);

    virtual ~OSDWidget() = default;

protected:
    void init_container(lv_obj_t* parent);
    void update_position();
    int abs_x, abs_y;
    lv_obj_t* container = nullptr;
    lv_obj_t* label = nullptr;
    lv_obj_t* icon = nullptr;

    std::string prefix;
    bool show_unit = true;
};
