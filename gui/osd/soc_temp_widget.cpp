#include "soc_temp_widget.h"
#include <cstdio>

SocTempWidget::SocTempWidget(lv_obj_t* parent, bool is_air, TelemetryState* state)
    : is_air_unit(is_air), telemetry_state(state) {

    init_container(parent);

    icon = nullptr;
    label = lv_label_create(container);
    lv_label_set_text(label, "--.-C");
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
}

void SocTempWidget::update() {
    if (!label || !telemetry_state) return;

    auto stats = is_air_unit ? telemetry_state->get_air_stats()
                              : telemetry_state->get_ground_stats();

    std::string formatted = prefix.empty() ? "" : prefix;
    formatted += std::to_string(stats.soc_temp_c);
    if (show_unit) {
        formatted += "C";
    }

    lv_label_set_text(label, formatted.c_str());
}

void SocTempWidget::set_color(lv_color_t color) {
    if (!style_initialized) {
        lv_style_init(&style);
        style_initialized = true;
    }

    lv_style_set_text_color(&style, color);
    lv_obj_add_style(label, &style, 0);
}
