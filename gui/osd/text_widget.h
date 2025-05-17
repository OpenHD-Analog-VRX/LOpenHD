#pragma once

#include "osd_widget.h"

class TextWidget : public OSDWidget {
public:
    TextWidget(lv_obj_t* parent, const std::string& text);

    void update() override {}
    void set_color(lv_color_t color) override;

private:
    std::string text_content;
    lv_style_t style;
    bool style_initialized = false;
};
