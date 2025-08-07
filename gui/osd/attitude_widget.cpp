#include "attitude_widget.h"
#include <lvgl.h>
#include <cmath>

AttitudeWidget::AttitudeWidget(lv_obj_t* parent, TelemetryState* state, int size)
    : telemetry_state(state), size(size) {

    init_container(parent);

    line1 = lv_line_create(container);
    line2 = lv_line_create(container);

    lv_obj_set_style_line_width(line1, 2, 0);
    lv_obj_set_style_line_color(line1, line_color, 0);
    lv_obj_set_style_line_opa(line1, LV_OPA_COVER, 0);

    lv_obj_set_style_line_width(line2, 2, 0);
    lv_obj_set_style_line_color(line2, line_color, 0);
    lv_obj_set_style_line_opa(line2, LV_OPA_COVER, 0);

    lv_obj_set_pos(line1, 0, 0);
    lv_obj_set_pos(line2, 0, 0);
}

void AttitudeWidget::set_color(lv_color_t color) {
    line_color = color;

    lv_obj_set_style_line_color(line1, color, 0);
    lv_obj_set_style_line_color(line2, color, 0);
}

void AttitudeWidget::update() {
    if (!telemetry_state) return;

    auto fc = telemetry_state->get_fc_stats();
    float pitch_rad = fc.pitch_deg;
    float roll_rad  = fc.roll_deg;

    int cx = size / 2;
    int cy = size / 2;

    int full_len = static_cast<int>(size * 0.7f);
    int gap      = static_cast<int>(size * 0.15f);

    int half = full_len / 2;
    int g = gap / 2;

    // normalize pitch: max pitch range ±1.2 rad (~70 deg)
    int pitch_offset = static_cast<int>((pitch_rad / 1.2f) * (size / 2 - 5));

    // rotation for roll
    float cos_r = std::cos(-roll_rad);
    float sin_r = std::sin(-roll_rad);

    // line midpoint (where the "gap" is), offset by pitch
    int mid_x = cx;
    int mid_y = cy + pitch_offset;

    // left part
    points1[0].x = mid_x - g * cos_r;
    points1[0].y = mid_y + g * sin_r;
    points1[1].x = mid_x - half * cos_r;
    points1[1].y = mid_y + half * sin_r;

    // right part
    points2[0].x = mid_x + g * cos_r;
    points2[0].y = mid_y - g * sin_r;
    points2[1].x = mid_x + half * cos_r;
    points2[1].y = mid_y - half * sin_r;

    lv_line_set_points(line1, points1, 2);
    lv_line_set_points(line2, points2, 2);
    update_position();
}
