#pragma once
#include "osd_widget.h"
#include <telemetry_state.h>

class BatteryWidget : public OSDWidget {
public:
    BatteryWidget(lv_obj_t* parent, bool is_air, TelemetryState* state);

    void update() override;
    void set_color(lv_color_t color) override;

private:
    lv_obj_t* label_current;
    lv_obj_t* label_percentage;
    lv_obj_t* label_voltage;
    bool is_air_unit;
    TelemetryState* telemetry_state;
    lv_style_t style;
    bool style_initialized = false;
};
