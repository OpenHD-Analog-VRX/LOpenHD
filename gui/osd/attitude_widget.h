#pragma once

#include "osd_widget.h"
#include "telemetry_state.h"

class AttitudeWidget : public OSDWidget {
public:
    AttitudeWidget(lv_obj_t* parent, TelemetryState* state, int size = 120);

    void update() override;
    void set_color(lv_color_t color) override;


private:
    lv_obj_t* canvas = nullptr;
    TelemetryState* telemetry_state = nullptr;

    int size = 120;
    lv_color_t line_color;
    lv_obj_t* line1 = nullptr;
    lv_obj_t* line2 = nullptr;

    lv_point_precise_t points1[2];
    lv_point_precise_t points2[2];
};
