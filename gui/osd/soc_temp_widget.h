#pragma once
#include "osd_widget.h"
#include <telemetry_state.h>

class SocTempWidget : public OSDWidget {
public:
    SocTempWidget(lv_obj_t* parent, bool is_air, TelemetryState* state);

    void update() override;
    void set_color(lv_color_t color) override;

private:
    lv_obj_t* label;
    bool is_air_unit;
    TelemetryState* telemetry_state;
    lv_style_t style;
    bool style_initialized = false;
};
