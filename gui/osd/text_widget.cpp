#include "text_widget.h"
#include "lvgl.h"

TextWidget::TextWidget(lv_obj_t* parent, const std::string& text)
    : text_content(text) {

    init_container(parent);

    label = lv_label_create(container);
    lv_label_set_text(label, text_content.c_str());
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
}

void TextWidget::set_color(lv_color_t color) {
    if (!style_initialized) {
        lv_style_init(&style);
        style_initialized = true;
    }
    lv_style_set_text_color(&style, color);
    lv_obj_add_style(label, &style, 0);
}
