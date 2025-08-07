#include "battery_widget.h"
#include <cstdio>

BatteryWidget::BatteryWidget(lv_obj_t* parent, bool is_air, TelemetryState* state)
    : is_air_unit(is_air), telemetry_state(state) {

    init_container(parent);

    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    icon = nullptr;
    label_current = lv_label_create(container);
    label_percentage = lv_label_create(container);
    label_voltage = lv_label_create(container);
    lv_label_set_text(label_current, "--.-A");
    lv_label_set_text(label_percentage, "--%");
    lv_label_set_text(label_voltage, "--.-V");
}

void BatteryWidget::update() {
    if (!telemetry_state) return;

    auto stats = telemetry_state->get_fc_stats();

    std::string formatted = prefix.empty() ? "" : prefix;
    formatted += std::to_string(stats.battery_remaining_pct);
    if (show_unit) {
        formatted += "%";
    }

    lv_label_set_text(label_percentage, formatted.c_str());

    formatted = prefix.empty() ? "" : prefix;
    formatted += std::to_string(stats.battery_current);
    if (show_unit) {
        formatted += "A";
    }

    lv_label_set_text(label_current, formatted.c_str());

    formatted = prefix.empty() ? "" : prefix;
    formatted += std::to_string(stats.battery_voltage);
    if (show_unit) {
        formatted += "V";
    }

    lv_label_set_text(label_voltage, formatted.c_str());

}

void BatteryWidget::set_color(lv_color_t color) {
    if (!style_initialized) {
        lv_style_init(&style);
        style_initialized = true;
    }

    lv_style_set_text_color(&style, color);
    lv_obj_add_style(label_voltage, &style, 0);
    lv_obj_add_style(label_percentage, &style, 0);
    lv_obj_add_style(label_current, &style, 0);
}
